/*
 * ConnectivityMonitoring
 * Generated on: 2023-11-02 16:57:15
 * Created by: Sinai RnD
 */

#include "o_4_connectivity_monitoring/ConnectivityMonitoring.h"

#include "Resource.h"
#include "ItemOp.h"
#include "WppTypes.h"
#include "WppLogs.h"
#include "WppClient.h"

/* --------------- Code_cpp block 0 start --------------- */
#define NTWRK_BRR_MIN 0
#define NTWRK_BRR_MAX 50
#define AVLB_NTWRK_BRR_MIN 0
#define AVLB_NTWRK_BRR_MAX 50
#define LINK_QUALITY_IEEE_802_15_4_MIN 0
#define LINK_QUALITY_IEEE_802_15_4_MAX 255
#define LINK_QUALITY_GSM_MIN 0
#define LINK_QUALITY_GSM_MAX 7
#if RES_4_6
#define LINK_UTLZTN_MIN 0
#define LINK_UTLZTN_MAX 100
#endif
#define CELL_ID_GSM_MIN	0
#define CELL_ID_GSM_MAX	65535
#define CELL_ID_WCDMA_MIN 0
#define CELL_ID_WCDMA_MAX 268435455
#if RES_4_9
#define SMNC_MIN 0
#define SMNC_MAX 999
#endif
#if RES_4_10
#define SMCC_MIN 0
#define SMCC_MAX 999
#endif

#ifdef OPENWRT_BUILD
#include "OpenWrtConnectivityInfo.h"
#endif

/* --------------- Code_cpp block 0 end --------------- */

#define TAG "ConnectivityMonitoring"

namespace wpp {

ConnectivityMonitoring::ConnectivityMonitoring(lwm2m_context_t &context, const OBJ_LINK_T &id): Instance(context, id) {

	/* --------------- Code_cpp block 1 start --------------- */
	/* --------------- Code_cpp block 1 end --------------- */

	resourcesCreate();
	resourcesInit();

	/* --------------- Code_cpp block 2 start --------------- */
	/* --------------- Code_cpp block 2 end --------------- */
}

ConnectivityMonitoring::~ConnectivityMonitoring() {
	/* --------------- Code_cpp block 3 start --------------- */
	/* --------------- Code_cpp block 3 end --------------- */
}

Object & ConnectivityMonitoring::object(WppClient &ctx) {
	return ctx.registry().connectivityMonitoring();
}

ConnectivityMonitoring * ConnectivityMonitoring::instance(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().connectivityMonitoring().instance(instId);
	if (!inst) return NULL;
	return static_cast<ConnectivityMonitoring*>(inst);
}

ConnectivityMonitoring * ConnectivityMonitoring::createInst(WppClient &ctx, ID_T instId) {
	Instance *inst = ctx.registry().connectivityMonitoring().createInstance(instId);
	if (!inst) return NULL;
	return static_cast<ConnectivityMonitoring*>(inst);
}

bool ConnectivityMonitoring::removeInst(WppClient &ctx, ID_T instId) {
	return ctx.registry().connectivityMonitoring().remove(instId);
}

void ConnectivityMonitoring::serverOperationNotifier(Instance *securityInst, ItemOp::TYPE type, const ResLink &resLink) {
	/* --------------- Code_cpp block 4 start --------------- */
	/* --------------- Code_cpp block 4 end --------------- */

	operationNotify(*this, resLink, type);

	/* --------------- Code_cpp block 5 start --------------- */
	/* --------------- Code_cpp block 5 end --------------- */
}

void ConnectivityMonitoring::userOperationNotifier(ItemOp::TYPE type, const ResLink &resLink) {
	if (type == ItemOp::WRITE || type == ItemOp::DELETE) notifyResChanged(resLink.resId, resLink.resInstId);

	/* --------------- Code_cpp block 6 start --------------- */
	/* --------------- Code_cpp block 6 end --------------- */
}

void ConnectivityMonitoring::resourcesCreate() {
	std::vector<Resource> resources = {
		{NETWORK_BEARER_0,              ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT },    
		{AVAILABLE_NETWORK_BEARER_1,    ItemOp(ItemOp::READ), IS_SINGLE::MULTIPLE, IS_MANDATORY::MANDATORY, TYPE_ID::INT },    
		{RADIO_SIGNAL_STRENGTH_2,       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::MANDATORY, TYPE_ID::INT },    
		#if RES_4_3                                                                                                                                             
		{LINK_QUALITY_3,                ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		{IP_ADDRESSES_4,                ItemOp(ItemOp::READ), IS_SINGLE::MULTIPLE, IS_MANDATORY::MANDATORY, TYPE_ID::STRING }, 
		#if RES_4_5                                                                                                                                             
		{ROUTER_IP_ADDRESSES_5,         ItemOp(ItemOp::READ), IS_SINGLE::MULTIPLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING }, 
		#endif                                                                                                                                                  
		#if RES_4_6                                                                                                                                             
		{LINK_UTILIZATION_6,            ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_7                                                                                                                                             
		{APN_7,                         ItemOp(ItemOp::READ), IS_SINGLE::MULTIPLE, IS_MANDATORY::OPTIONAL,  TYPE_ID::STRING }, 
		#endif                                                                                                                                                  
		#if RES_4_8                                                                                                                                             
		{CELL_ID_8,                     ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_9                                                                                                                                             
		{SMNC_9,                        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_10                                                                                                                                            
		{SMCC_10,                       ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_11                                                                                                                                            
		{SIGNALSNR_11,                  ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_12                                                                                                                                            
		{LAC_12,                        ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
		#if RES_4_13                                                                                                                                            
		{COVERAGE_ENHANCEMENT_LEVEL_13, ItemOp(ItemOp::READ), IS_SINGLE::SINGLE,   IS_MANDATORY::OPTIONAL,  TYPE_ID::INT },    
		#endif                                                                                                                                                  
	};
	setupResources(std::move(resources));
}

void ConnectivityMonitoring::resourcesInit() {
	/* --------------- Code_cpp block 7 start --------------- */

	#ifdef OPENWRT_BUILD
	resource(NETWORK_BEARER_0)->set<INT_T>(OpenWrtConnectivityInfo::getNetworkBearer());
	#else
	resource(NETWORK_BEARER_0)->set<INT_T>(NTWRK_BRR_MAX);
	#endif
	resource(NETWORK_BEARER_0)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return GSM <= value && value < NTWRK_BRR_MAX; });

	resource(AVAILABLE_NETWORK_BEARER_1)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return AVLB_NTWRK_BRR_MIN <= value && value <= AVLB_NTWRK_BRR_MAX; });
	#ifdef OPENWRT_BUILD
	std::vector<int> bearers = OpenWrtConnectivityInfo::getAvailableNetworkBearers();
	for (size_t i = 0; i < bearers.size(); i++) {
		resource(AVAILABLE_NETWORK_BEARER_1)->set<INT_T>(bearers[i], i);
	}
	#endif

	#ifdef OPENWRT_BUILD
	resource(RADIO_SIGNAL_STRENGTH_2)->set<INT_T>(OpenWrtConnectivityInfo::getRadioSignalStrength());
	#else
	resource(RADIO_SIGNAL_STRENGTH_2)->set<INT_T>(0);
	#endif

	#if RES_4_3
	#ifdef OPENWRT_BUILD
	resource(LINK_QUALITY_3)->set<INT_T>(OpenWrtConnectivityInfo::getLinkQuality());
	#else
	resource(LINK_QUALITY_3)->set<INT_T>(0);
	#endif
	resource(LINK_QUALITY_3)->setDataVerifier((VERIFY_INT_T)[this](const INT_T& value) { return this->checkLinkQuality(value); });
	#endif

	#ifdef OPENWRT_BUILD
	// Set IP addresses
	std::vector<std::string> ipAddrs = OpenWrtConnectivityInfo::getIpAddresses();
	for (size_t i = 0; i < ipAddrs.size(); i++) {
		resource(IP_ADDRESSES_4)->set<STRING_T>(ipAddrs[i], i);
	}
	#endif

	#if RES_4_5
	#ifdef OPENWRT_BUILD
	// Set router/gateway IP addresses
	std::vector<std::string> routerAddrs = OpenWrtConnectivityInfo::getRouterIpAddresses();
	for (size_t i = 0; i < routerAddrs.size(); i++) {
		resource(ROUTER_IP_ADDRESSES_5)->set<STRING_T>(routerAddrs[i], i);
	}
	#endif
	#endif

	#if RES_4_6
	#ifdef OPENWRT_BUILD
	resource(LINK_UTILIZATION_6)->set<INT_T>(OpenWrtConnectivityInfo::getLinkUtilization());
	#else
	resource(LINK_UTILIZATION_6)->set<INT_T>(LINK_UTLZTN_MIN);
	#endif
	resource(LINK_UTILIZATION_6)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return LINK_UTLZTN_MIN <= value && value <= LINK_UTLZTN_MAX; });
	#endif

	#if RES_4_7
	#ifdef OPENWRT_BUILD
	// Set APN (for cellular)
	std::vector<std::string> apns = OpenWrtConnectivityInfo::getAPN();
	for (size_t i = 0; i < apns.size(); i++) {
		resource(APN_7)->set<STRING_T>(apns[i], i);
	}
	#endif
	#endif

	#if RES_4_8
	#ifdef OPENWRT_BUILD
	resource(CELL_ID_8)->set<INT_T>(OpenWrtConnectivityInfo::getCellId());
	#else
	resource(CELL_ID_8)->set<INT_T>(0);
	#endif
	resource(CELL_ID_8)->setDataVerifier((VERIFY_INT_T)[this](const INT_T& value) { return this->checkCellId(value); });
	#endif

	#if RES_4_9
	#ifdef OPENWRT_BUILD
	resource(SMNC_9)->set<INT_T>(OpenWrtConnectivityInfo::getSMNC());
	#else
	resource(SMNC_9)->set<INT_T>(SMNC_MIN);
	#endif
	resource(SMNC_9)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return SMNC_MIN <= value && value <= SMNC_MAX; });
	#endif

	#if RES_4_10
	#ifdef OPENWRT_BUILD
	resource(SMCC_10)->set<INT_T>(OpenWrtConnectivityInfo::getSMCC());
	#else
	resource(SMCC_10)->set<INT_T>(SMCC_MIN);
	#endif
	resource(SMCC_10)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return SMCC_MIN <= value && value <= SMCC_MAX; });
	#endif

	#if RES_4_11
	resource(SIGNALSNR_11)->set<INT_T>(0);
	#endif

	#if RES_4_12
	resource(LAC_12)->set<INT_T>(0);
	#endif

	#if RES_4_13
	resource(COVERAGE_ENHANCEMENT_LEVEL_13)->set<INT_T>(CVRG_ENHNCMNT_LVL_MAX);
	resource(COVERAGE_ENHANCEMENT_LEVEL_13)->setDataVerifier((VERIFY_INT_T)[](const INT_T& value) { return MISSING <= value && value < CVRG_ENHNCMNT_LVL_MAX; });
	#endif

	/* --------------- Code_cpp block 7 end --------------- */
}

/* --------------- Code_cpp block 8 start --------------- */
bool ConnectivityMonitoring::checkLinkQuality(uint8_t linkQuality) {
	INT_T networkBearer = resource(NETWORK_BEARER_0)->get<INT_T>();
    switch (networkBearer) {
        case GSM:
			return linkQuality <= LINK_QUALITY_GSM_MAX;  // MIN is 0, uint8_t is always >= 0
        case IEEE_802_15_4:
            return true;  // uint8_t range is 0-255, MAX is 255, always valid
		case LTE_TDD:
		case LTE_FDD:
		case NB_IOT:
			// TODO: the RSRQ used in the first 2 cases and NRSRQ in third. But measures in dB
			// TODO: not understund the range of available values
			return true;
		default:
			// return true as for as resource not used in the another cases
			return true;
	}
}

bool ConnectivityMonitoring::checkCellId(uint32_t cellId) {
	INT_T networkBearer = resource(NETWORK_BEARER_0)->get<INT_T>();
    switch (networkBearer) {
        case GSM:
            return cellId <= CELL_ID_GSM_MAX;  // MIN is 0, uint32_t is always >= 0
        case WCDMA:
		case TD_SCDMA:
			return cellId <= CELL_ID_WCDMA_MAX;  // MIN is 0, uint32_t is always >= 0
		default:
			// return true as for as resource not used in the another cases
			return true;
	}
}
/* --------------- Code_cpp block 8 end --------------- */

} /* namespace wpp */
