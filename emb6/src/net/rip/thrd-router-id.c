/*
 * thrd-router-id.c
 *
 *  Created on: 23 May 2016
 *  Author: Lukas Zimmermann <lzimmer1@stud.hs-offenburg.de>
 *  Router ID Management / Router ID Assignment.
 */

#include "emb6.h"
#include "bsp.h"
#include "thread_conf.h"
#include "ctimer.h"

#include "er-coap.h"
#include "er-coap-engine.h"
#include "rest-engine.h"

#include "thrd-router-id.h"
#include "thrd-leader-db.h"
#include "thrd-partition.h"

#include "thrd-route.h"

#define DEBUG DEBUG_PRINT
#include "uip-debug.h"	// For debugging terminal output.

/*=============================================================================
                               Router ID Management
===============================================================================*/

static char *service_urls[2] =
{ "a/as", "a/ar"};

static struct ctimer leader_ct;
// static struct ctimer router_ct;

/**
 * Address Query Payload Buffer (MUST be at least 38 octets).
 */
static uint8_t addr_solicit_buf[24] = { 0 };

static coap_packet_t packet[1]; /* This way the packet can be treated as pointer as usual. */

static size_t len = 0;						// CoAP payload length.
static tlv_t *tlv;
static net_tlv_status_t *status_tlv;
static net_tlv_rloc16_t *rloc16_tlv;
static net_tlv_router_mask_t *router_mask_tlv;

/*
 * CoAP Resources to be activated need to be imported through the extern keyword.
 */
extern resource_t
	thrd_res_a_as;

/* --------------------------------------------------------------------------- */

/**
 * Initialize CoAP resources.
 */
static void coap_init();

static void handle_timer(void *ptr);

static size_t create_addr_solicit_req_payload(uint8_t *buf, uint8_t *ml_eid,
		uint16_t *rloc16);

/* --------------------------------------------------------------------------- */

/**
 * The Leader maintains an ID_Assignment_Set containing the IEEE 802.15.4
 * Extended Address of each device that has an assigned ID and, for currently
 * unassigned IDs, the time at which the ID may be reassigned.
 */

void
thrd_leader_init(void)
{
	// Starting a new Partition as the Leader.
	thrd_dev.type = THRD_DEV_TYPE_LEADER;

	thrd_ldb_init(); // TODO Call this function during compile process.
	thrd_ldb_ida_empty();	// Empty ID Assignment Set.

	// Get a Router ID.
	uint8_t desired_rid = 0;
	thrd_ldb_ida_t *ida = thrd_leader_assign_rid(&desired_rid, 0);	// TODO Add my MAC Extended Address here.
	if ( ida != NULL ) {
		thrd_dev.Router_ID = ida->ID_id;
	}

	coap_init();

	ctimer_set(&leader_ct, (clock_time_t)thrd_next_period(ID_SEQUENCE_PERIOD), handle_timer, NULL);
}

/* --------------------------------------------------------------------------- */

static void
coap_init()
{
	PRINTF("Router ID Assignment: Registering CoAP resources.\n\r");

	// Bind the resources to their Uri-Path.
	rest_activate_resource(&thrd_res_a_as, "a/as");
}

/* --------------------------------------------------------------------------- */

static void
handle_timer(void *ptr)
{
	ID_sequence_number++;
	thrd_print_partition_data();
	ctimer_set(&leader_ct, (clock_time_t)thrd_next_period(ID_SEQUENCE_PERIOD), handle_timer, NULL);
}

/* --------------------------------------------------------------------------- */

clock_time_t
thrd_next_period(uint8_t sec)
{
	return (clock_time_t)(sec * bsp_get(E_BSP_GET_TRES));
}

/* --------------------------------------------------------------------------- */

thrd_ldb_ida_t
*thrd_leader_assign_rid(uint8_t *router_id, uint64_t id_owner)
{
	thrd_ldb_ida_t *ida;

	// Check whether desired Router ID is available.
	ida = thrd_ldb_ida_lookup(*router_id);
	if ( ida == NULL ) {
		ida = thrd_ldb_ida_add(*router_id, id_owner, 0);
		return ida;
	} else {
	// If the desired Router ID currently is in use.
		clock_time_t now = bsp_get(E_BSP_GET_SEC);
		// Check reassignment delay.
		if ( ida->ID_reuse_time == 0 || ida->ID_reuse_time < now ) {
			// Router ID is not available --> Looking for an unassigned Router ID.
			uint8_t id_cnt = 0;
			ida = thrd_ldb_ida_lookup(id_cnt);

			while ( ida != NULL && id_cnt < 63 ) {
				// Check whether the current Router ID is in use.
				if ( ida->ID_reuse_time != 0 ) {
					break;
				}
				id_cnt++;
				ida = thrd_ldb_ida_lookup(id_cnt);
			}
			// Create Router ID if available.
			if ( id_cnt < 63 ) {
				ida = thrd_ldb_ida_add(id_cnt, id_owner, 0);
				return ida;
			} else {
				return NULL;
			}
		} else {
			// Reassignment delay expired --> Router ID available.
			ida->ID_owner = id_owner;
			ida->ID_reuse_time = 0;
			return ida;
		}
	}
	return NULL;
}

/* --------------------------------------------------------------------------- */

void
thrd_leader_dealloc_rid(uint8_t router_id)
{
	thrd_rdb_id_t *rid;
	thrd_ldb_ida_t *ida;
	rid = thrd_rdb_rid_lookup(router_id);
	ida = thrd_ldb_ida_lookup(router_id);

	if ( rid != NULL && ida != NULL ) {
		// Removing the router id from the Router ID Set.
		thrd_rdb_rid_rm(rid);
		// Set the router id's reuse time.
		ida->ID_reuse_time = thrd_next_period(ID_REUSE_DELAY);
	}
}

/*=============================================================================
                               Router ID Assignment
===============================================================================*/

void
thrd_request_router_id(uip_ipaddr_t *leader_addr, uint8_t *ml_eid, uint8_t *router_id)
{
	uint16_t rloc16 = THRD_CREATE_RLOC16(*router_id, 0); // Create RLOC16.
	len = create_addr_solicit_req_payload(addr_solicit_buf, ml_eid, &rloc16);

	coap_init_message(packet, COAP_TYPE_CON, COAP_POST, 0);
	coap_set_header_uri_path(packet, service_urls[0]);
	coap_set_payload(packet, addr_solicit_buf, len);
	coap_nonblocking_request(leader_addr, UIP_HTONS(COAP_DEFAULT_PORT), packet, thrd_addr_solicit_chunk_handler); // TODO Changing CoAP Port.
}

/* --------------------------------------------------------------------------- */

static size_t
create_addr_solicit_req_payload(uint8_t *buf, uint8_t *ml_eid, uint16_t *rloc16)
{
	if ( buf != NULL && ml_eid != NULL ) {
		// Create ML-EID TLV.
		buf[0] = NET_TLV_ML_EID;
		buf[1] = 8;
		memcpy(&buf[2], ml_eid, 8);
		if ( rloc16 != NULL ) {
			// Create RLOC16 TLV.
			buf[10] = NET_TLV_RLOC16;
			buf[11] = 2;
			memcpy(&buf[12], rloc16, 2);
			return 14;
		}
		return 10;
	}
	return 0;
}

/* --------------------------------------------------------------------------- */

/* This function is will be passed to coap_nonblocking_request() to handle responses. */
void
thrd_addr_solicit_chunk_handler(void *response)
{
	PRINTF("thrd_addr_solicit_chunk_handler: Received response!");
    const uint8_t *chunk;
    if ( !response ) {

    } else {
    	int payload_len = coap_get_payload(response, &chunk);
    	if ( payload_len >= 3 ) {
    		// TODO Process payload -> Receipt of Address Solicit Response.
    		tlv = (tlv_t*) &chunk[0];
    		if ( tlv->type == NET_TLV_STATUS && tlv->length == 1 ) {
    			status_tlv = (net_tlv_status_t*) tlv->value;
    			PRINTF("Status = %d\n", status_tlv->status);
    		}
    		if ( status_tlv->status == 0 && payload_len == 16 ) {
    			// Success.
    			tlv = (tlv_t*) &chunk[3];
    			if ( tlv->type == NET_TLV_RLOC16 && tlv->length == 2 ) {
    				rloc16_tlv = (net_tlv_rloc16_t*) tlv->value;
    				PRINTF("RLOC16 = %04x\n", rloc16_tlv->rloc16);
    			}
    			tlv = (tlv_t*) &chunk[7];
    			if ( tlv->type == NET_TLV_ROUTER_MASK && tlv->length == 7 ) {
    				router_mask_tlv = (net_tlv_router_mask_t*) tlv->value;
    				PRINTF("ID Sequence Number = %d\n", router_mask_tlv->id_sequence_number);
    				PRINTF("Router ID Mask = %16x\n", router_mask_tlv->router_id_mask);
    			}
    		}
    	}
    }
}

/* --------------------------------------------------------------------------- */

uint64_t
thrd_create_router_id_mask()
{
	thrd_rdb_id_t *rid;
	// Router ID Mask and Link Quality and Router Data.
	uint64_t router_id_mask = 0x0000000000000000;
	for ( rid = thrd_rdb_rid_head(); rid != NULL; rid = thrd_rdb_rid_next(rid) ) {
		router_id_mask |= (0x8000000000000000 >> rid->router_id);
	}
	return router_id_mask;
}

/* --------------------------------------------------------------------------- */

