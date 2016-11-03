/**
 * \file thrd-dev.c
 * \author Institute for reliable Embedded Systems and Communication Electronics
 * \date 2016/06/15
 * \version 1.0
 *
 * \brief Thread device
 */

#include "thread_conf.h"
#include "thrd-dev.h"
#include "thrd-iface.h"
#include "thrd-leader-db.h"
#include "er-coap.h"
#include "er-coap-engine.h"
#include "rest-engine.h"
#include "thrd-iface.h"
#include "thrd-partition.h"

#define     LOGGER_ENABLE                 LOGGER_THRD_NET
#include    "logger.h"

/** Thread default Device Type Configuration. */
thrd_dev_t thrd_dev = {

#ifdef THRD_DEV_NETTYPE
		.net_type = THRD_DEV_NETTYPE,
#else
		.net_type = THRD_DEV_NETTYPE_REED,
#endif
#ifdef THRD_DEV_LLTYPE
		.ll_type = THRD_DEV_LLTYPE,
#else
		.ll_type = THRD_DEV_LLTYPE_FFD,
#endif
};

/*==============================================================================
                          LOCAL VARIABLE DECLARATIONS
 =============================================================================*/

/*==============================================================================
                               LOCAL FUNCTION PROTOTYPES
 =============================================================================*/

/*==============================================================================
                                    LOCAL FUNCTIONS
 =============================================================================*/

/* --------------------------------------------------------------------------- */

/*==============================================================================
                                     API FUNCTIONS
 =============================================================================*/

void
thrd_dev_init(void)
{
#if (THRD_DEV_TYPE == THRD_DEV_TYPE_REED) || (THRD_DEV_TYPE == THRD_DEV_TYPE_ROUTER)
	/* Receives all CoAP messages */
	coap_init_engine();
	/* Initialize the REST engine. */
	rest_init_engine();
#endif
}

/* --------------------------------------------------------------------------- */

thrd_dev_type_t
thrd_get_dev_net_type(void)
{
	return thrd_dev.net_type;
}

/* --------------------------------------------------------------------------- */

thrd_error_t
thrd_set_dev_net_type(thrd_dev_type_t type)
{
	switch(type) {
	case THRD_DEV_NETTYPE_ROUTER:
		thrd_dev.net_type = THRD_DEV_NETTYPE_ROUTER;
		break;
	case THRD_DEV_NETTYPE_REED:
		thrd_dev.net_type = THRD_DEV_NETTYPE_REED;
		break;
	case THRD_DEV_NETTYPE_PED:
		thrd_dev.net_type = THRD_DEV_NETTYPE_PED;
		break;
	case THRD_DEV_NETTYPE_SED:
		thrd_dev.net_type = THRD_DEV_NETTYPE_SED;
		break;
	case THRD_DEV_NETTYPE_LEADER:
		thrd_dev.net_type = THRD_DEV_NETTYPE_LEADER;
		break;
	default:
		thrd_dev.net_type = THRD_DEV_NETTYPE_REED;
		return THRD_ERROR_INVALID_ARGS;
	}
	return THRD_ERROR_NONE;
}

/* --------------------------------------------------------------------------- */

/*==============================================================================
                                    DEBUG FUNCTIONS
 =============================================================================*/

void
thrd_dev_print_dev_info()
{
	LOG_RAW("|==================================== THREAD DEVICE ====================================|\n\r");
	LOG_RAW("| Device type = ");
	switch(thrd_dev.net_type) {
	case THRD_DEV_NETTYPE_ROUTER:
		LOG_RAW("THRD_DEV_NETTYPE_ROUTER");
		break;
	case THRD_DEV_NETTYPE_REED:
		LOG_RAW("THRD_DEV_NETTYPE_REED");
		break;
	case THRD_DEV_NETTYPE_PED:
		LOG_RAW("THRD_DEV_NETTYPE_PED");
		break;
	case THRD_DEV_NETTYPE_SED:
		LOG_RAW("THRD_DEV_NETTYPE_SED");
		break;
	case THRD_DEV_NETTYPE_LEADER:
		LOG_RAW("THRD_DEV_NETTYPE_LEADER");
		break;
	default:
		LOG_RAW("THRD_DEV_NETTYPE_REED");
		break;
	}
	LOG_RAW("\n");
	// Print interface.
	thrd_iface_print();
	// Print Thread Partition data.
	thrd_print_partition_data();
	LOG_RAW("|=======================================================================================|\n\r");
}

