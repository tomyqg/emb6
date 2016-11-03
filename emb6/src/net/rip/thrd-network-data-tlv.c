/**
 * \file thrd-network-data-tlv.c
 * \author Institute for reliable Embedded Systems and Communication Electronics
 * \date 2016/06/12
 * \version 1.0
 *
 * \brief Thread network data message format
 */

/*
 ********************************************************************************
 *                                   INCLUDES
 ********************************************************************************
 */

#include "emb6.h"
#include "tlv.h"
#include "thrd-network-data-tlv.h"

/*
 ********************************************************************************
 *                          LOCAL FUNCTION DECLARATIONS
 ********************************************************************************
 */

/*
 ********************************************************************************
 *                               LOCAL VARIABLES
 ********************************************************************************
 */

/*
 ********************************************************************************
 *                               GLOBAL VARIABLES
 ********************************************************************************
 */

/*
 ********************************************************************************
 *                           LOCAL FUNCTION DEFINITIONS
 ********************************************************************************
 */

void
thrd_net_data_msg_tlv_setType(tlv_t *tlv, net_data_msg_tlv_type_t type)
{
	tlv->type = type << 1;
}

/* --------------------------------------------------------------------------- */

void
thrd_net_data_msg_tlv_setStable(tlv_t *tlv)
{
	tlv->type |= 0x01;
}

/* --------------------------------------------------------------------------- */

void
thrd_net_data_msg_tlv_setLength(tlv_t *tlv, uint8_t length)
{
	tlv->length = length;
}

/* --------------------------------------------------------------------------- */

net_data_msg_tlv_type_t
thrd_net_data_msg_getType(tlv_t *tlv)
{
	return tlv->type >> 1;
}

/* --------------------------------------------------------------------------- */

bool
thrd_net_data_msg_isStable(tlv_t *tlv)
{
	if ( tlv != NULL ) {
		uint8_t stable = tlv->type & 0x01;
		if ( stable == 0x01 ) {
			return TRUE;
		}
	}
	return FALSE;
}

/*
 ********************************************************************************
 *                           API FUNCTION DEFINITIONS
 ********************************************************************************
 */

/*
 ********************************************************************************
 *                               END OF FILE
 ********************************************************************************
 */
