

#include <bcomdef.h>
#include <gapgattserver.h>
#include <gap_advertiser.h>
#include <gapbondmgr.h>
#include "ti_ble_config.h"
#include <ti/bleapp/ble_app_util/inc/bleapputil_api.h>

// The GAP profile role
uint8_t profileRole = GAP_PROFILE_PERIPHERAL;
// GAP GATT Service (GGS) parameters
uint8_t attDeviceName[GAP_DEVICE_NAME_LEN]= "RFstar_76DA";

uint8_t pRandomAddress[B_ADDR_LEN] = {0};

/*********************************************************************
 * Bond Manager Configuration
 */

gapBondParams_t gapBondParams = {
    .pairMode             = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ,
    .mitm                 = false,
    .ioCap                = GAPBOND_IO_CAP_DISPLAY_ONLY,
    .bonding              = false,
    .secureConnection     = GAPBOND_SECURE_CONNECTION_ALLOW,
    .authenPairingOnly    = false,
    .autoSyncWL           = false,
    .eccReGenPolicy       = 0,
    .KeySize              = 16,
    .removeLRUBond        = false,
    .KeyDistList          = GAPBOND_KEYDIST_MENCKEY | GAPBOND_KEYDIST_MIDKEY | GAPBOND_KEYDIST_MSIGN | GAPBOND_KEYDIST_SENCKEY | GAPBOND_KEYDIST_SIDKEY | GAPBOND_KEYDIST_SSIGN,
    .eccDebugKeys         = false,
    .eraseBondWhileInConn = false,
    .sameIrkAction        = GAPBOND_SAME_IRK_UPDATE_BOND_REC
};

uint8_t pairMode                =    GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
uint8_t mitm                    =    false;
uint8_t ioCap                   =    GAPBOND_IO_CAP_DISPLAY_ONLY;
uint8_t bonding                 =    false;
uint8_t secureConnection        =    GAPBOND_SECURE_CONNECTION_ALLOW;
uint8_t authenPairingOnly       =    false;
uint8_t autoSyncWL              =    false;
uint8_t eccReGenPolicy          =    0;
uint8_t KeySize                 =    16;
uint8_t removeLRUBond           =    false;
uint8_t KeyDistList             =    GAPBOND_KEYDIST_MENCKEY | GAPBOND_KEYDIST_MIDKEY | GAPBOND_KEYDIST_MSIGN | GAPBOND_KEYDIST_SENCKEY | GAPBOND_KEYDIST_SIDKEY | GAPBOND_KEYDIST_SSIGN;
uint8_t allowDebugKeys          =    false;
uint8_t eraseBondWhileInConn    =    false;
uint8_t sameIrkAction           =    GAPBOND_SAME_IRK_UPDATE_BOND_REC;

void setBondManagerParameters()
{
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    GAPBondMgr_SetParameter(GAPBOND_SECURE_CONNECTION, sizeof(uint8_t), &secureConnection);
    // Set Authenticated Pairing Only mode
    GAPBondMgr_SetParameter(GAPBOND_AUTHEN_PAIRING_ONLY, sizeof(uint8_t), &authenPairingOnly);
    // Set Auto Acceptlist Sync
    GAPBondMgr_SetParameter(GAP_ADV_WL_POLICY_ANY_REQ, sizeof(uint8_t), &autoSyncWL);
    GAPBondMgr_SetParameter(GAPBOND_ECCKEY_REGEN_POLICY, sizeof(uint8_t), &eccReGenPolicy);
    GAPBondMgr_SetParameter(GAPBOND_KEYSIZE, sizeof(uint8_t), &KeySize);
    GAPBondMgr_SetParameter(GAPBOND_LRU_BOND_REPLACEMENT, sizeof(uint8_t), &removeLRUBond);
    GAPBondMgr_SetParameter(GAPBOND_KEY_DIST_LIST, sizeof(uint8_t), &KeyDistList);
    // Set Allow Debug Keys
    GAPBondMgr_SetParameter(GAPBOND_ALLOW_DEBUG_KEYS, sizeof(uint8_t), &allowDebugKeys);
    // Set the Erase bond While in Active Connection Flag
    GAPBondMgr_SetParameter(GAPBOND_ERASE_BOND_IN_CONN, sizeof(uint8_t), &eraseBondWhileInConn);
    GAPBondMgr_SetParameter(GAPBOND_SAME_IRK_OPTION, sizeof(uint8_t), &sameIrkAction);
}


GapAdv_params_t advParams1 = {
  .eventProps =   GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_SCANNABLE,
  .primIntMin =   160,
  .primIntMax =   480,
  .primChanMap =  GAP_ADV_CHAN_ALL,
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,
  .peerAddr =     { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,
  .txPower =      GAP_ADV_TX_POWER_NO_PREFERENCE,
  .primPhy =      GAP_ADV_PRIM_PHY_1_MBPS,
  .secPhy =       GAP_ADV_SEC_PHY_1_MBPS,
  .sid =          0
};

uint8_t advData1[] =
{
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED | GAP_ADTYPE_FLAGS_GENERAL,

  0x11,
  GAP_ADTYPE_128BIT_MORE,
  0x9e,
  0xca,
  0xdc,
  0x24,
  0x0e,
  0xe5,
  0xa9,
  0xe0,
  0x93,
  0xf3,
  0xa3,
  0xb5,
  0x01,
  0x00,
  0x40,
  0x6e,




  0x09,
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,
  //Company Identifier
  0xaa,
  0x55,

  //Additional Data
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,

};

uint8_t scanResData1[] =
{
  0x0c,
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'R',
  'F',
  's',
  't',
  'a',
  'r',
  '_',
  '7',
  '6',
  'D',
  'A',

  0x05,
  GAP_ADTYPE_16BIT_COMPLETE,
  LO_UINT16(0x0000),
  HI_UINT16(0x0000),
  LO_UINT16(0x0022),
  HI_UINT16(0x0022),



};

