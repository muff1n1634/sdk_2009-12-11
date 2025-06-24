#!/usr/bin/env python3

import os

def Object(
	lib: str,
	name: str,
	debug_matching: bool,
	release_matching: bool,
	# these are for extraction, don't change them
	*,
	debug_second: bool = False,
	release_second: bool = False,
	):
	obj_name = os.path.splitext(name)[0] + ".o"

	return [
		{
			# debug
			"extract": {
				"lib": lib + "D",
				"name": obj_name,
				"index": "2" if debug_second else "1",
			},

			"objdiff": {
				"name": lib + "D/" + name,
				"target_path": "obj/" + lib + "D/" + obj_name,
				"base_path": "build/" + lib + "D/" + obj_name,
				"complete": debug_matching,
			},
		},
		{
			# release
			"extract": {
				"lib": lib,
				"name": obj_name,
				"index": "2" if debug_second else "1",
			},

			"objdiff": {
				"name": lib + "/" + name,
				"target_path": "obj/" + lib + "/" + obj_name,
				"base_path": "build/" + lib + "/" + obj_name,
				"complete": release_matching,
			},
		},
	]

# flags

main_flags = [
	"-proc gekko",
	"-fp hardware",
	"-enum int",
	"-lang c99",
	"-cpp_exceptions off",
	"-cwd include",
]

debug_flags = [
	"-opt off",
	"-inline off",

	# Doesn't match because inlines are placed into a separate .text section,
	# but uncomment if you want line info

	# "-g",
]

release_flags = [
	"-O4,p",
	"-ipa file",
	"-DNDEBUG",
]

# objects

DebugMatching = True
DebugNonMatching = False
ReleaseMatching = True
ReleaseNonMatching = False

objs = []
objs += Object("ai",            "ai.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("amcExi",        "AmcExi2Stubs.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("arc",           "arc.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AX.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXAlloc.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXAux.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXCL.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXOut.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXSPB.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXVPB.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXProf.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "AXComp.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "DSPCode.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("ax",            "DSPADPCM.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axart.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axartsound.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axartcents.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axartenv.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axartlfo.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axart3d.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("axart",         "axartlpf.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbHi.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbHiDpl2.c",          DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbHiExp.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbHiExpDpl2.c",       DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXDelay.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXDelayDpl2.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXDelayExp.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXDelayExpDpl2.c",          DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbStd.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbStdDpl2.c",         DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbStdExp.c",          DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXReverbStdExpDpl2.c",      DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXChorus.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXChorusDpl2.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXChorusExp.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXChorusExpDpl2.c",         DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXLfoTable.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXSrcCoef.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("axfx",          "AXFXHooks.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("base",          "PPCArch.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("base",          "PPCPm.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gki_buffer.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gki_debug.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gki_time.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gki_ppc.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hcisu_h2.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "uusb_ppc.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_aa_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_ag_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_av_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_bi_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_cg_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_ct_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dg_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dm_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_fs_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_ft_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hd_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hh_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_pbc_cfg.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_pbs_cfg.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_pr_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_ss_cfg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_sys_cfg.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_disp.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_hcisu.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_init.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_load.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_logmsg.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_main.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bte_version.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btu_task1.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bd.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_sys_conn.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_sys_main.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "ptim.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "utl.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_prm_api.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_prm_main.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_fs_ci.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dm_act.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dm_api.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dm_main.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_dm_pm.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hd_act.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hd_api.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hd_main.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hh_act.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hh_api.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hh_main.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "bta_hh_utils.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_acl.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_dev.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_devctl.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_discovery.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_inq.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_main.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_pm.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_sco.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btm_sec.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btu_hcif.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "btu_init.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "wbt_ext.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gap_api.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gap_conn.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "gap_utils.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "goep_trace.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "goep_util.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "goep_fs.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hcicmds.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidd_api.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidd_conn.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidd_mgmt.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidd_pm.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidh_api.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "hidh_conn.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "l2c_api.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "l2c_csm.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "l2c_link.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "l2c_main.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "l2c_utils.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "port_api.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "port_rfc.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "port_utils.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_l2cap_if.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_mx_fsm.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_port_fsm.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_port_if.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_ts_frames.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "rfc_utils.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_api.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_db.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_discovery.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_main.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_server.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "sdp_utils.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "xml_bld.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("bte",           "xml_parse.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("cx",            "CXCompression.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("cx",            "CXStreamingUncompression.c",  DebugNonMatching, ReleaseNonMatching)
objs += Object("cx",            "CXUncompression.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("cx",            "CXSecureUncompression.c",     DebugNonMatching, ReleaseNonMatching)
objs += Object("darch",         "darch.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("db",            "db.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("dsp",           "dsp.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("dsp",           "dsp_debug.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("dsp",           "dsp_perf.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("dsp",           "dsp_task.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvdfs.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvd.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvdqueue.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvderror.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvdidutils.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvdFatal.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvdDeviceError.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("dvd",           "dvd_broadway.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "encutility.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "encunicode.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "encjapanese.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "enclatin.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "encconvert.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "encchinese.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("enc",           "enckorean.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("esp",           "esp.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("euart",         "euart.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("exi",           "EXIBios.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("exi",           "EXIUart.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("exi",           "EXIAd16.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("exi",           "EXICommon.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("fs",            "fs.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXInit.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXFifo.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXAttr.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXMisc.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXGeometry.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXFrameBuf.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXLight.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXTexture.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXBump.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXTev.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXPixel.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXDraw.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXDisplayList.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXVert.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXTransform.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXVerify.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXVerifXF.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXVerifRAS.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXSave.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("gx",            "GXPerf.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_api.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_ios.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_client.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_device.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_interface.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_open_close.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("hid",           "hid_task.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMFrameController.cpp",      DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMAnmController.cpp",        DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMGUIManager.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMController.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMRemoteSpk.cpp",            DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMAxSound.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMCommon.cpp",               DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "HBMBase.cpp",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_animation.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_arcResourceAccessor.cpp", DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_bounding.cpp",            DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_common.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_drawInfo.cpp",            DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_group.cpp",               DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_init.cpp",                DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_layout.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_material.cpp",            DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_pane.cpp",                DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_picture.cpp",             DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_resourceAccessor.cpp",    DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_textBox.cpp",             DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "lyt_window.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "math_arithmetic.cpp",         DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "math_equation.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "math_geometry.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "math_triangular.cpp",         DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "math_types.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_binaryFileFormat.cpp",     DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_CharStrmReader.cpp",       DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_CharWriter.cpp",           DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_Font.cpp",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_LinkList.cpp",             DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_list.cpp",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_ResFont.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_ResFontBase.cpp",          DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_RomFont.cpp",              DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_TagProcessorBase.cpp",     DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "ut_TextWriterBase.cpp",       DebugNonMatching, ReleaseNonMatching)
objs += Object("homebuttonLib", "mix.cpp",                     DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "syn.cpp",                     DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synctrl.cpp",                 DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synenv.cpp",                  DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synmix.cpp",                  DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synpitch.cpp",                DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synsample.cpp",               DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "synvoice.cpp",                DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("homebuttonLib", "seq.cpp",                     DebugNonMatching, ReleaseNonMatching, release_second = True)
objs += Object("ipc",           "ipcMain.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("ipc",           "ipcclt.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("ipc",           "memory.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("ipc",           "ipcProfile.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib_led.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib_init.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib_maps_us.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib_maps_jp.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("kbd",           "kbd_lib_maps_eu.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("kpad",          "KPAD.c",                      DebugMatching   , ReleaseMatching   )
objs += Object("kpad",          "KMPLS.c",                     DebugMatching   , ReleaseMatching   )
objs += Object("kpad",          "KZMplsTestSub.c",             DebugMatching   , ReleaseMatching   )
objs += Object("kpr",           "kpr_lib.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_heapCommon.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_expHeap.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_frameHeap.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_unitHeap.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_allocator.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("mem",           "mem_list.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("midi",          "MIDI.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("midi",          "MIDIRead.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("midi",          "MIDIXfer.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("mix",           "mix.c",                       DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("mix",           "remote.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "mtx.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "mtxvec.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "mtxstack.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "mtx44.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "mtx44vec.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "vec.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "quat.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("mtx",           "psmtx.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "nand.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDOpenClose.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDCore.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDSecret.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDCheck.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDLogging.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("nand",          "NANDErrorMessage.c",          DebugNonMatching, ReleaseNonMatching)
objs += Object("NdevExi2A",     "DebuggerDriver.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("NdevExi2A",     "exi2.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("odenotstub",    "odenotstub.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OS.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSAddress.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSAlarm.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSAlloc.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSArena.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSAudioSystem.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSCache.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSContext.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSError.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSExec.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSFatal.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSFont.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSInterrupt.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSLink.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSMessage.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSMemory.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSMutex.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSReboot.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSReset.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSRtc.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSSemaphore.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSStopwatch.c",               DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSSync.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSThread.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSTime.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSTimer.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSUtf.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSIpc.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSStateTM.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "__start.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSPlayRecord.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSStateFlags.c",              DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSNet.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSNandbootInfo.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSPlayTime.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSInstall.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSCrc.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "OSLaunch.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("os",            "__ppc_eabi_init.c",           DebugNonMatching, ReleaseNonMatching)
objs += Object("pad",           "Padclamp.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("pad",           "Pad.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("rso",           "RSOLink.c",                   DebugNonMatching, ReleaseNonMatching)
objs += Object("sc",            "scsystem.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("sc",            "scapi.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("sc",            "scapi_net.c",                 DebugNonMatching, ReleaseNonMatching)
objs += Object("sc",            "scapi_prdinfo.c",             DebugNonMatching, ReleaseNonMatching)
objs += Object("seq",           "seq.c",                       DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("si",            "SIBios.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("si",            "SISamplingRate.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("si",            "SISteering.c",                DebugNonMatching, ReleaseNonMatching)
objs += Object("si",            "SISteeringXfer.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("si",            "SISteeringAuto.c",            DebugNonMatching, ReleaseNonMatching)
objs += Object("sp",            "sp.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("syn",           "syn.c",                       DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synctrl.c",                   DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synenv.c",                    DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synlfo.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("syn",           "synmix.c",                    DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synpitch.c",                  DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synsample.c",                 DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synvoice.c",                  DebugNonMatching, ReleaseNonMatching, debug_second = True)
objs += Object("syn",           "synwt.c",                     DebugNonMatching, ReleaseNonMatching)
objs += Object("thp",           "THPDec.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("thp",           "THPStats.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("thp",           "THPAudio.c",                  DebugNonMatching, ReleaseNonMatching)
objs += Object("tpl",           "TPL.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("usb",           "usb.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("vi",            "vi.c",                        DebugNonMatching, ReleaseNonMatching)
objs += Object("vi",            "i2c.c",                       DebugNonMatching, ReleaseNonMatching)
objs += Object("vi",            "vi3in1.c",                    DebugNonMatching, ReleaseNonMatching)
objs += Object("wenc",          "wenc.c",                      DebugNonMatching, ReleaseNonMatching)
objs += Object("wpad",          "WPAD.c",                      DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "WPADHIDParser.c",             DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "WPADEncrypt.c",               DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "WPADClamp.c",                 DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "WPADMem.c",                   DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "lint.c",                      DebugMatching,    ReleaseNonMatching)
objs += Object("wpad",          "WUD.c",                       DebugMatching,    ReleaseMatching   )
objs += Object("wpad",          "WUDHidHost.c",                DebugMatching,    ReleaseMatching   )


# TODO: move these out
def extract_objects(objs):
	import subprocess

	os.makedirs("obj/")

	for obj_ in objs:
		obj = obj_["extract"]

		obj_dir = "obj/" + obj["lib"]
		obj_name = obj["name"]

		os.makedirs(obj_dir, exist_ok = True)
		subprocess.run([
			"ar",
			"xN",
			obj["index"],
			"orig/slamWiiD.a",
			obj_name,
			"--output",
			obj_dir
		])
		os.chmod(obj_dir + "/" + obj_name, 0o644) # u+rw, g+r, a+r

		asm_dir = "asm/" + obj["lib"]
		asm_name = os.path.splitext(obj_name)[0] + ".s"

		subprocess.run([
			"dtk",
			"-L","error",
			"elf",
			"disasm",
			obj_dir + "/" + obj_name,
			asm_dir + "/" + asm_name
		])


def write_objdiff_json(objs):
	import json

	with open("objdiff.json", "w") as file:
		json_object = {
			"build_base": False,
			"units": [obj_["objdiff"] for obj_ in objs]
		}

		file.write(json.dumps(json_object, indent = 2))


def main():
	import sys

	if not sys.argv[1] or sys.argv[1] == "extract":
		if not os.path.isdir("obj/"):
			extract_objects(objs)

	if not sys.argv[1] or sys.argv[1] == "objdiff":
		write_objdiff_json(objs)


if __name__ == "__main__":
	main()
