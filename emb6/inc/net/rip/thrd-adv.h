/**
 * \file thrd-adv.h
 * \author Institute for reliable Embedded Systems and Communication Electronics
 * \date 2016/05/10
 * \version 1.0
 *
 * \brief MLE advertisement processing / Route64 TLV generation
 */

#ifndef EMB6_INC_NET_RIP_THRD_ADV_H_
#define EMB6_INC_NET_RIP_THRD_ADV_H_

#include "tlv.h"

// TODO Check!
#define MAX_ROUTE64_TLV_DATA_SIZE		41		// One plus ceiling (MAX_ROUTER_ID/8) plus the number of assigned router IDs.

// Route64 data masks.
#define ROUTE64_LQ_RD_IN_MASK			0x30
#define ROUTE64_LQ_RD_ROUTE_MASK		0x0F

#define ROUTE64_LQ_RD_INVALID			0x01

/*==============================================================================
                                     API FUNCTIONS
 =============================================================================*/

/**
 * Process MLE Advertisement.
 * @param source_rloc The sender's RLOC (Source Address TLV).
 * @param route64_tlv The Route64 TLV.
 * @param leader_tlv The Leader Data TLV.
 */
void thrd_process_adv(uint16_t source_rloc, tlv_route64_t *route64_tlv, tlv_leader_t *leader_tlv);

tlv_route64_t* thrd_generate_route64(size_t *len);

tlv_leader_t* thrd_generate_leader_data_tlv(void);

void print_leader_data_tlv(tlv_leader_t * leader_tlv);

#endif /* EMB6_INC_NET_RIP_THRD_ADV_H_ */
