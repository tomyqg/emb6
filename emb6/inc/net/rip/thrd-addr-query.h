/**
 * \file thrd-addr-query.h
 * \author Institute for reliable Embedded Systems and Communication Electronics
 * \date 2016/06/06
 * \version 1.0
 *
 * \brief Thread address queries
 */

#ifndef EMB6_INC_NET_RIP_THRD_ADDR_QUERY_H_
#define EMB6_INC_NET_RIP_THRD_ADDR_QUERY_H_

#include "emb6.h"
#include "clist.h"
#include "uip.h"
#include "net_tlv.h"
#include "ctimer.h"

#define THRD_AQ_TIMEOUT						2		// Value not specified in Thread Specification (v1.0).
#define THRD_AQ_INITIAL_RETRY_DELAY			2		// Value not specified in Thread Specification (v1.0).

/* Number of URIs that can be queried. */
#define NUMBER_OF_URLS 3

extern void thrd_eid_rloc_db_init(void);

/* --------------------------------------------------------------------------- */

/**
 * Local Address Set.
 */
typedef struct thrd_local_addr {
	struct thrd_local_addr *next;

	uip_ipaddr_t EID;
} thrd_local_addr_t;

/* --------------------------------------------------------------------------- */

/**
 * RFD Child Address Set.
 * Set for child addresses.
 */
typedef struct thrd_rfd_child_addr {
	struct thrd_rfd_child_addr *next;

	uip_ipaddr_t childAddr;
} thrd_rfd_child_addr_t;

/**
 * RFD Child Address Set.
 * Pointers to Address Sets for all different scopes.
 */
typedef struct thrd_rfd_child_set {
	thrd_rfd_child_addr_t *childAddr;
} thrd_rfd_child_set_t;

/* --------------------------------------------------------------------------- */

/**
 * Address Query Set.
 */
typedef struct thrd_addr_qry {
	struct thrd_addr_qry *next;

	uip_ipaddr_t EID;
	clock_time_t AQ_Timeout;
	uint8_t AQ_Failures;
	clock_time_t AQ_Retry_Delay;
	struct ctimer timer;
} thrd_addr_qry_t;

/* --------------------------------------------------------------------------- */
/* --------------------------- Local Address Set ----------------------------- */
/* --------------------------------------------------------------------------- */

/**
 * Get the number of the currently stored EIDs in the Local Address Set.
 * @return The number of currently stored EIDs.
 */
uint8_t thrd_local_addr_num();

/**
 * Get a pointer to the first element of the Local Address Set.
 * @return A pointer to the first element.
 */
thrd_local_addr_t *thrd_local_addr_head(void);

/**
 * Get the subsequent element of the given entry in the Local Address Set.
 * @param i A thrd_local_addr_t element.
 * @return A pointer to the subsequent element.
 */
thrd_local_addr_t *thrd_local_addr_next(thrd_local_addr_t *i);

/**
 * Look up a given EID in the Local Address Set.
 * @param eid An Endpoint Identifier.
 * @return The corresponding thrd_local_addr_t entry (if exist).
 */
thrd_local_addr_t *thrd_local_addr_lookup(uip_ipaddr_t eid);

/**
 * Add a given EID to the Local Address Set.
 * @param eid An Endpoint Identifier.
 * @return A pointer to the currently inserted thrd_local_addr_t entry.
 */
thrd_local_addr_t *thrd_local_addr_add(uip_ipaddr_t eid);

/**
 * Remove a given EID element of the Local Address Set.
 * @param localAddr An EID element.
 */
void thrd_local_addr_rm(thrd_local_addr_t *localAddr);

/**
 * Empty Local Address Set.
 */
void thrd_local_addr_empty();

/* --------------------------------------------------------------------------- */
/* ------------------------- RFD Child Address Set --------------------------- */
/* --------------------------------------------------------------------------- */

/**
 * RFD Child IPv6 Prefix type.
 */
typedef enum {
	THRD_RFD_CHILD_PREFIX_LL = 0,//!< THRD_RFD_CHILD_PREFIX_LL
	THRD_RFD_CHILD_PREFIX_ML = 1,//!< THRD_RFD_CHILD_PREFIX_ML
	THRD_RFD_CHILD_PREFIX_OS = 2,//!< THRD_RFD_CHILD_PREFIX_OS
} thrd_rfd_child_prefix_type_t;

/**
 * Get the number of currently stored RFD Child Addresses in the
 * RFD Child Address Set (Link-Local Scope addresses).
 * @return The number of currently stored RFD Child Addresses (LL).
 */
size_t thrd_rfd_child_ll_addr_num();

/**
 * Get the number of currently stored RFD Child Addresses in the
 * RFD Child Address Set (Other-Scope addresses).
 * @return The number of currently stored RFD Child Addresses (OS).
 */
size_t thrd_rfd_child_ml_addr_num();


/**
 * Get the number of currently stored RFD Child Addresses in the
 * RFD Child Address Set (Linl-Local Scope addresses).
 * @return The number of currently stored RFD Child Addresses (LL).
 */
size_t thrd_rfd_child_os_addr_num();

/**
 * Get a pointer to the first element of the RFD Child Address Set.
 * @param addr_type The address type as thrd_rfd_child_prefix_type_t.
 * @return A pointer to the first element.
 */
thrd_rfd_child_addr_t *thrd_rfd_child_addr_head(thrd_rfd_child_prefix_type_t addr_type);

/**
 * Get the subsequent element of the given entry in the RFD Child Address Set.
 * @param i A thrd_rfd_addr_t element.
 * @return A pointer to the subsequent element.
 */
thrd_rfd_child_addr_t *thrd_rfd_child_addr_next(thrd_rfd_child_addr_t *i);

/**
 * Look up a given RFD Child Address entry in the RFD Child Address Set.
 * @param child_addr The Endpoint Identifier of the child.
 * @return The corresponding thrd_rfd_child_addr_t entry (if exist).
 */
thrd_rfd_child_addr_t *thrd_rfd_child_addr_lookup(uip_ipaddr_t child_addr);

/**
 * Add a given RFD Child Address to the RFD Child Address Set.
 * @param child_addr The Endpoint Identifier of the child.
 * @return A pointer to the currently inserted thrd_rfd_child_addr_t entry.
 */
thrd_rfd_child_addr_t *thrd_rfd_child_addr_add(uip_ipaddr_t child_addr);

/**
 * Remove a given RFD Child Address element of the RFD Child Address Set.
 * @param child_addr A thrd_rfd_addr_t element.
 */
void thrd_rfd_child_addr_rm(thrd_rfd_child_addr_t *child_addr);

/**
 * Empty RFD Child Address Set.
 */
void thrd_rfd_child_addr_empty();

/* --------------------------------------------------------------------------- */
/* --------------------------- Address Query Set ----------------------------- */
/* --------------------------------------------------------------------------- */

uint8_t thrd_addr_qry_num();

thrd_addr_qry_t *thrd_addr_qry_head(void);

thrd_addr_qry_t *thrd_addr_qry_next(thrd_addr_qry_t *i);

thrd_addr_qry_t *thrd_addr_qry_lookup(uip_ipaddr_t eid);

thrd_addr_qry_t *thrd_addr_qry_add(uip_ipaddr_t eid, clock_time_t timeout, clock_time_t retry_delay);

/**
 * Remove a given Address uery element of the Address Query Set.
 * @param child_addr A thrd_addr_qr_t element.
 */
void thrd_addr_qry_rm(thrd_addr_qry_t *addrQr);

/**
 * Empty Address Query Set.
 */
void thrd_addr_qry_empty();

/* --------------------------------------------------------------------------- */
/* ----------------------------- Address Query ------------------------------- */
/* --------------------------------------------------------------------------- */

/**
 * Send an Address Query Request message.
 * @param target_eid The Target EID.
 */
void thrd_addr_qry_request(uip_ipaddr_t *target_eid);

/**
 * Send an Address Notification Response message.
 * @param dest_addr The destination IPv6 address.
 * @param target_eid The Target EID.
 * @param rloc16 The corresponding RLOC16.
 * @param ml_eid_tlv The corresonding ML-EID of the target.
 * @param last_trans_time The Last Transaction Time (Optional). NULL if not used.
 */
void thrd_addr_ntf_response(uip_ipaddr_t *dest_addr, uip_ipaddr_t *target_eid, uint16_t *rloc16,
		uint8_t *ml_eid, clock_time_t *last_trans_time);

/**
 * Send an Address Error Notification message.
 * @param dest_addr The destination IPv6 address.
 * @param target_eid The Target EID.
 * @param ml_eid The corresponding ML-EID.
 */
void thrd_addr_err_ntf_send(uip_ipaddr_t *dest_addr, uip_ipaddr_t *target_eid,
		uint8_t *ml_eid);

/* --------------------------------------------------------------------------- */
/* --------------------------------- DEBUG ----------------------------------- */
/* --------------------------------------------------------------------------- */

#if RIP_DEBUG
void thrd_local_addr_set_print(void);
void thrd_rfd_child_addr_set_print();
void thrd_addr_qr_set_print();
#endif /* RIP_DEBUG */


#endif /* EMB6_INC_NET_RIP_THRD_ADDR_QUERY_H_ */

