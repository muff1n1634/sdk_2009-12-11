# extract objects from slamWii archives

.PHONY: extract extract-debug disasm-debug extract-release disasm-release

extract: disasm-debug disasm-release

extract-debug:
ifneq ($(wildcard obj/debug),)
	$(info Debug libraries already extracted.)
else
ifeq ($(wildcard orig/slamWiiD.a),)
	$(error orig/slamWiiD.a not found)
endif
	$(info Extracting debug libraries from orig/slamWiiD.a to obj/debug.)
	@mkdir -p $(foreach d,obj asm,$(addprefix $d/debug/,NdevExi2A amcExi odenotstub tpl thp homebuttonLib ai arc axart ax axfx base rso bte cx db dsp dvd enc esp euart exi fs gx ipc kpad mem mix mtx nand os pad sc seq si sp syn usb vi wenc wpad midi darch hid kbd kpr))
	@${AR} x orig/slamWiiD.a DebuggerDriver.o exi2.o --output obj/debug/NdevExi2A
	@${AR} x orig/slamWiiD.a AmcExi2Stubs.o --output obj/debug/amcExi
	@${AR} x orig/slamWiiD.a odenotstub.o --output obj/debug/odenotstub
	@${AR} x orig/slamWiiD.a TPL.o --output obj/debug/tpl
	@${AR} x orig/slamWiiD.a THPDec.o THPStats.o THPAudio.o --output obj/debug/thp
	@${AR} x orig/slamWiiD.a HBMFrameController.o HBMAnmController.o HBMGUIManager.o HBMController.o HBMRemoteSpk.o HBMAxSound.o HBMCommon.o HBMBase.o lyt_animation.o lyt_arcResourceAccessor.o lyt_bounding.o lyt_common.o lyt_drawInfo.o lyt_group.o lyt_init.o lyt_layout.o lyt_material.o lyt_pane.o lyt_picture.o lyt_resourceAccessor.o lyt_textBox.o lyt_window.o math_arithmetic.o math_equation.o math_geometry.o math_triangular.o math_types.o ut_binaryFileFormat.o ut_CharStrmReader.o ut_CharWriter.o ut_Font.o ut_LinkList.o ut_list.o ut_ResFont.o ut_ResFontBase.o ut_RomFont.o ut_TagProcessorBase.o ut_TextWriterBase.o mix.o syn.o synctrl.o synenv.o synmix.o synpitch.o synsample.o synvoice.o seq.o --output obj/debug/homebuttonLib
	@${AR} x orig/slamWiiD.a ai.o --output obj/debug/ai
	@${AR} x orig/slamWiiD.a arc.o --output obj/debug/arc
	@${AR} x orig/slamWiiD.a axart.o axartsound.o axartcents.o axartenv.o axartlfo.o axart3d.o axartlpf.o --output obj/debug/axart
	@${AR} x orig/slamWiiD.a AX.o AXAlloc.o AXAux.o AXCL.o AXOut.o AXSPB.o AXVPB.o AXProf.o AXComp.o DSPCode.o DSPADPCM.o --output obj/debug/ax
	@${AR} x orig/slamWiiD.a AXFXReverbHi.o AXFXReverbHiDpl2.o AXFXReverbHiExp.o AXFXReverbHiExpDpl2.o AXFXDelay.o AXFXDelayDpl2.o AXFXDelayExp.o AXFXDelayExpDpl2.o AXFXReverbStd.o AXFXReverbStdDpl2.o AXFXReverbStdExp.o AXFXReverbStdExpDpl2.o AXFXChorus.o AXFXChorusDpl2.o AXFXChorusExp.o AXFXChorusExpDpl2.o AXFXLfoTable.o AXFXSrcCoef.o AXFXHooks.o --output obj/debug/axfx
	@${AR} x orig/slamWiiD.a PPCArch.o PPCPm.o --output obj/debug/base
	@${AR} x orig/slamWiiD.a RSOLink.o --output obj/debug/rso
	@${AR} x orig/slamWiiD.a gki_buffer.o gki_debug.o gki_time.o gki_ppc.o hcisu_h2.o uusb_ppc.o bta_aa_cfg.o bta_ag_cfg.o bta_av_cfg.o bta_bi_cfg.o bta_cg_cfg.o bta_ct_cfg.o bta_dg_cfg.o bta_dm_cfg.o bta_fs_cfg.o bta_ft_cfg.o bta_hd_cfg.o bta_hh_cfg.o bta_pbc_cfg.o bta_pbs_cfg.o bta_pr_cfg.o bta_ss_cfg.o bta_sys_cfg.o bte_disp.o bte_hcisu.o bte_init.o bte_load.o bte_logmsg.o bte_main.o bte_version.o btu_task1.o bd.o bta_sys_conn.o bta_sys_main.o ptim.o utl.o bta_prm_api.o bta_prm_main.o bta_fs_ci.o bta_dm_act.o bta_dm_api.o bta_dm_main.o bta_dm_pm.o bta_hd_act.o bta_hd_api.o bta_hd_main.o bta_hh_act.o bta_hh_api.o bta_hh_main.o bta_hh_utils.o btm_acl.o btm_dev.o btm_devctl.o btm_discovery.o btm_inq.o btm_main.o btm_pm.o btm_sco.o btm_sec.o btu_hcif.o btu_init.o wbt_ext.o gap_api.o gap_conn.o gap_utils.o goep_trace.o goep_util.o goep_fs.o hcicmds.o hidd_api.o hidd_conn.o hidd_mgmt.o hidd_pm.o hidh_api.o hidh_conn.o l2c_api.o l2c_csm.o l2c_link.o l2c_main.o l2c_utils.o port_api.o port_rfc.o port_utils.o rfc_l2cap_if.o rfc_mx_fsm.o rfc_port_fsm.o rfc_port_if.o rfc_ts_frames.o rfc_utils.o sdp_api.o sdp_db.o sdp_discovery.o sdp_main.o sdp_server.o sdp_utils.o xml_bld.o xml_parse.o --output obj/debug/bte
	@${AR} x orig/slamWiiD.a CXCompression.o CXStreamingUncompression.o CXUncompression.o CXSecureUncompression.o --output obj/debug/cx
	@${AR} x orig/slamWiiD.a db.o --output obj/debug/db
	@${AR} x orig/slamWiiD.a dsp.o dsp_debug.o dsp_perf.o dsp_task.o --output obj/debug/dsp
	@${AR} x orig/slamWiiD.a dvdfs.o dvd.o dvdqueue.o dvderror.o dvdidutils.o dvdFatal.o dvdDeviceError.o dvd_broadway.o --output obj/debug/dvd
	@${AR} x orig/slamWiiD.a encutility.o encunicode.o encjapanese.o enclatin.o encconvert.o encchinese.o enckorean.o --output obj/debug/enc
	@${AR} x orig/slamWiiD.a esp.o --output obj/debug/esp
	@${AR} x orig/slamWiiD.a euart.o --output obj/debug/euart
	@${AR} x orig/slamWiiD.a EXIBios.o EXIUart.o EXIAd16.o EXICommon.o --output obj/debug/exi
	@${AR} x orig/slamWiiD.a fs.o --output obj/debug/fs
	@${AR} x orig/slamWiiD.a GXInit.o GXFifo.o GXAttr.o GXMisc.o GXGeometry.o GXFrameBuf.o GXLight.o GXTexture.o GXBump.o GXTev.o GXPixel.o GXDraw.o GXDisplayList.o GXVert.o GXTransform.o GXVerify.o GXVerifXF.o GXVerifRAS.o GXSave.o GXPerf.o --output obj/debug/gx
	@${AR} x orig/slamWiiD.a ipcMain.o ipcclt.o memory.o ipcProfile.o --output obj/debug/ipc
	@${AR} x orig/slamWiiD.a KPAD.o KMPLS.o KZMplsTestSub.o --output obj/debug/kpad
	@${AR} x orig/slamWiiD.a mem_heapCommon.o mem_expHeap.o mem_frameHeap.o mem_unitHeap.o mem_allocator.o mem_list.o --output obj/debug/mem
	@${AR} xN 2 orig/slamWiiD.a mix.o --output obj/debug/mix
	@${AR} x orig/slamWiiD.a remote.o --output obj/debug/mix
	@${AR} x orig/slamWiiD.a mtx.o mtxvec.o mtxstack.o mtx44.o mtx44vec.o vec.o quat.o psmtx.o --output obj/debug/mtx
	@${AR} x orig/slamWiiD.a nand.o NANDOpenClose.o NANDCore.o NANDSecret.o NANDCheck.o NANDLogging.o NANDErrorMessage.o --output obj/debug/nand
	@${AR} x orig/slamWiiD.a OS.o OSAddress.o OSAlarm.o OSAlloc.o OSArena.o OSAudioSystem.o OSCache.o OSContext.o OSError.o OSExec.o OSFatal.o OSFont.o OSInterrupt.o OSLink.o OSMessage.o OSMemory.o OSMutex.o OSReboot.o OSReset.o OSRtc.o OSSemaphore.o OSStopwatch.o OSSync.o OSThread.o OSTime.o OSTimer.o OSUtf.o OSIpc.o OSStateTM.o __start.o OSPlayRecord.o OSStateFlags.o OSNet.o OSNandbootInfo.o OSPlayTime.o OSInstall.o OSCrc.o OSLaunch.o __ppc_eabi_init.o --output obj/debug/os
	@${AR} x orig/slamWiiD.a Padclamp.o Pad.o --output obj/debug/pad
	@${AR} x orig/slamWiiD.a scsystem.o scapi.o scapi_net.o scapi_prdinfo.o --output obj/debug/sc
	@${AR} xN 2 orig/slamWiiD.a seq.o --output obj/debug/seq
	@${AR} x orig/slamWiiD.a SIBios.o SISamplingRate.o SISteering.o SISteeringXfer.o SISteeringAuto.o --output obj/debug/si
	@${AR} x orig/slamWiiD.a sp.o --output obj/debug/sp
	@${AR} xN 2 orig/slamWiiD.a syn.o synctrl.o synenv.o --output obj/debug/syn
	@${AR} x orig/slamWiiD.a synlfo.o --output obj/debug/syn
	@${AR} xN 2 orig/slamWiiD.a synmix.o synpitch.o synsample.o synvoice.o --output obj/debug/syn
	@${AR} x orig/slamWiiD.a synwt.o --output obj/debug/syn
	@${AR} x orig/slamWiiD.a usb.o --output obj/debug/usb
	@${AR} x orig/slamWiiD.a vi.o i2c.o vi3in1.o --output obj/debug/vi
	@${AR} x orig/slamWiiD.a wenc.o --output obj/debug/wenc
	@${AR} x orig/slamWiiD.a WPAD.o WPADHIDParser.o WPADEncrypt.o WPADClamp.o WPADMem.o lint.o WUD.o WUDHidHost.o --output obj/debug/wpad
	@${AR} x orig/slamWiiD.a MIDI.o MIDIRead.o MIDIXfer.o --output obj/debug/midi
	@${AR} x orig/slamWiiD.a darch.o --output obj/debug/darch
	@${AR} x orig/slamWiiD.a hid_api.o hid_ios.o hid_client.o hid_device.o hid_interface.o hid_open_close.o hid_task.o --output obj/debug/hid
	@${AR} x orig/slamWiiD.a kbd_lib.o kbd_lib_led.o kbd_lib_init.o kbd_lib_maps_us.o kbd_lib_maps_jp.o kbd_lib_maps_eu.o --output obj/debug/kbd
	@${AR} x orig/slamWiiD.a kpr_lib.o --output obj/debug/kpr
	@${FIND} obj/debug -type f -exec chmod +r {} \;
endif

ifneq ($(wildcard asm/debug),)
disasm-debug:; $(info Debug libraries already extracted.)
else
disasm-debug: extract-debug
	@$(foreach f,$(wildcard obj/debug/*/*.o),dtk -L error elf disasm $f ${f:obj/%.o=asm/%.s};)
endif

extract-release:
ifneq ($(wildcard obj/release),)
	$(info Release libraries already extracted.)
else
ifeq ($(wildcard orig/slamWii.a),)
	$(error orig/slamWii.a not found)
endif
	$(info Extracting release libraries from orig/slamWii.a to obj/release.)
	@mkdir -p $(foreach d,obj asm,$(addprefix $d/release/,base db os sp axart mtx vi pad ai ax axfx mix syn seq dsp gx exi si NdevExi2A mem euart fs ipc nand sc wenc arc cx enc wpad bte usb kpad esp rso midi darch hid kbd kpr amcExi odenotstub tpl thp homebuttonLib homebuttonLib dvd))
	@${AR} x orig/slamWii.a PPCArch.o PPCPm.o --output obj/release/base
	@${AR} x orig/slamWii.a db.o --output obj/release/db
	@${AR} x orig/slamWii.a OS.o OSAddress.o OSAlarm.o OSAlloc.o OSArena.o OSAudioSystem.o OSCache.o OSContext.o OSError.o OSExec.o OSFatal.o OSFont.o OSInterrupt.o OSLink.o OSMessage.o OSMemory.o OSMutex.o OSReboot.o OSReset.o OSRtc.o OSSemaphore.o OSStopwatch.o OSSync.o OSThread.o OSTime.o OSTimer.o OSUtf.o OSIpc.o OSStateTM.o time.dolphin.o __start.o OSPlayRecord.o OSStateFlags.o OSNet.o OSNandbootInfo.o OSPlayTime.o OSInstall.o OSCrc.o OSLaunch.o __ppc_eabi_init.o --output obj/release/os
	@${AR} x orig/slamWii.a sp.o --output obj/release/sp
	@${AR} x orig/slamWii.a axart.o axartsound.o axartcents.o axartenv.o axartlfo.o axart3d.o axartlpf.o --output obj/release/axart
	@${AR} x orig/slamWii.a mtx.o mtxvec.o mtxstack.o mtx44.o mtx44vec.o vec.o quat.o psmtx.o --output obj/release/mtx
	@${AR} x orig/slamWii.a vi.o i2c.o vi3in1.o --output obj/release/vi
	@${AR} x orig/slamWii.a Padclamp.o Pad.o --output obj/release/pad
	@${AR} x orig/slamWii.a ai.o --output obj/release/ai
	@${AR} x orig/slamWii.a AX.o AXAlloc.o AXAux.o AXCL.o AXOut.o AXSPB.o AXVPB.o AXProf.o AXComp.o DSPCode.o DSPADPCM.o --output obj/release/ax
	@${AR} x orig/slamWii.a AXFXReverbHi.o AXFXReverbHiDpl2.o AXFXReverbHiExp.o AXFXReverbHiExpDpl2.o AXFXDelay.o AXFXDelayDpl2.o AXFXDelayExp.o AXFXDelayExpDpl2.o AXFXReverbStd.o AXFXReverbStdDpl2.o AXFXReverbStdExp.o AXFXReverbStdExpDpl2.o AXFXChorus.o AXFXChorusDpl2.o AXFXChorusExp.o AXFXChorusExpDpl2.o AXFXLfoTable.o AXFXSrcCoef.o AXFXHooks.o --output obj/release/axfx
	@${AR} x orig/slamWii.a mix.o remote.o --output obj/release/mix
	@${AR} x orig/slamWii.a syn.o synctrl.o synenv.o synlfo.o synmix.o synpitch.o synsample.o synvoice.o synwt.o --output obj/release/syn
	@${AR} x orig/slamWii.a seq.o --output obj/release/seq
	@${AR} x orig/slamWii.a dsp.o dsp_debug.o dsp_perf.o dsp_task.o --output obj/release/dsp
	@${AR} x orig/slamWii.a GXInit.o GXFifo.o GXAttr.o GXMisc.o GXGeometry.o GXFrameBuf.o GXLight.o GXTexture.o GXBump.o GXTev.o GXPixel.o GXDraw.o GXDisplayList.o GXVert.o GXTransform.o GXVerify.o GXVerifXF.o GXVerifRAS.o GXSave.o GXPerf.o --output obj/release/gx
	@${AR} x orig/slamWii.a EXIBios.o EXIUart.o EXIAd16.o EXICommon.o --output obj/release/exi
	@${AR} x orig/slamWii.a SIBios.o SISamplingRate.o SISteering.o SISteeringXfer.o SISteeringAuto.o --output obj/release/si
	@${AR} x orig/slamWii.a DebuggerDriver.o exi2.o --output obj/release/NdevExi2A
	@${AR} x orig/slamWii.a mem_heapCommon.o mem_expHeap.o mem_frameHeap.o mem_unitHeap.o mem_allocator.o mem_list.o --output obj/release/mem
	@${AR} x orig/slamWii.a euart.o --output obj/release/euart
	@${AR} x orig/slamWii.a fs.o --output obj/release/fs
	@${AR} x orig/slamWii.a ipcMain.o ipcclt.o memory.o ipcProfile.o --output obj/release/ipc
	@${AR} x orig/slamWii.a nand.o NANDOpenClose.o NANDCore.o NANDSecret.o NANDCheck.o NANDLogging.o NANDErrorMessage.o --output obj/release/nand
	@${AR} x orig/slamWii.a scsystem.o scapi.o scapi_net.o scapi_prdinfo.o --output obj/release/sc
	@${AR} x orig/slamWii.a wenc.o --output obj/release/wenc
	@${AR} x orig/slamWii.a arc.o --output obj/release/arc
	@${AR} x orig/slamWii.a CXCompression.o CXStreamingUncompression.o CXUncompression.o CXSecureUncompression.o --output obj/release/cx
	@${AR} x orig/slamWii.a encutility.o encunicode.o encjapanese.o enclatin.o encconvert.o encchinese.o enckorean.o --output obj/release/enc
	@${AR} x orig/slamWii.a WPAD.o WPADHIDParser.o WPADEncrypt.o WPADClamp.o WPADMem.o lint.o WUD.o WUDHidHost.o --output obj/release/wpad
	@${AR} x orig/slamWii.a gki_buffer.o gki_debug.o gki_time.o gki_ppc.o hcisu_h2.o uusb_ppc.o bta_aa_cfg.o bta_ag_cfg.o bta_av_cfg.o bta_bi_cfg.o bta_cg_cfg.o bta_ct_cfg.o bta_dg_cfg.o bta_dm_cfg.o bta_fs_cfg.o bta_ft_cfg.o bta_hd_cfg.o bta_hh_cfg.o bta_pbc_cfg.o bta_pbs_cfg.o bta_pr_cfg.o bta_ss_cfg.o bta_sys_cfg.o bte_disp.o bte_hcisu.o bte_init.o bte_load.o bte_logmsg.o bte_main.o bte_version.o btu_task1.o bd.o bta_sys_conn.o bta_sys_main.o ptim.o utl.o bta_prm_api.o bta_prm_main.o bta_fs_ci.o bta_dm_act.o bta_dm_api.o bta_dm_main.o bta_dm_pm.o bta_hd_act.o bta_hd_api.o bta_hd_main.o bta_hh_act.o bta_hh_api.o bta_hh_main.o bta_hh_utils.o btm_acl.o btm_dev.o btm_devctl.o btm_discovery.o btm_inq.o btm_main.o btm_pm.o btm_sco.o btm_sec.o btu_hcif.o btu_init.o wbt_ext.o gap_api.o gap_conn.o gap_utils.o goep_trace.o goep_util.o goep_fs.o hcicmds.o hidd_api.o hidd_conn.o hidd_mgmt.o hidd_pm.o hidh_api.o hidh_conn.o l2c_api.o l2c_csm.o l2c_link.o l2c_main.o l2c_utils.o port_api.o port_rfc.o port_utils.o rfc_l2cap_if.o rfc_mx_fsm.o rfc_port_fsm.o rfc_port_if.o rfc_ts_frames.o rfc_utils.o sdp_api.o sdp_db.o sdp_discovery.o sdp_main.o sdp_server.o sdp_utils.o xml_bld.o xml_parse.o --output obj/release/bte
	@${AR} x orig/slamWii.a usb.o --output obj/release/usb
	@${AR} x orig/slamWii.a KPAD.o KMPLS.o KZMplsTestSub.o --output obj/release/kpad
	@${AR} x orig/slamWii.a esp.o --output obj/release/esp
	@${AR} x orig/slamWii.a RSOLink.o --output obj/release/rso
	@${AR} x orig/slamWii.a MIDI.o MIDIRead.o MIDIXfer.o --output obj/release/midi
	@${AR} x orig/slamWii.a darch.o --output obj/release/darch
	@${AR} x orig/slamWii.a hid_api.o hid_ios.o hid_client.o hid_device.o hid_interface.o hid_open_close.o hid_task.o --output obj/release/hid
	@${AR} x orig/slamWii.a kbd_lib.o kbd_lib_led.o kbd_lib_init.o kbd_lib_maps_us.o kbd_lib_maps_jp.o kbd_lib_maps_eu.o --output obj/release/kbd
	@${AR} x orig/slamWii.a kpr_lib.o --output obj/release/kpr
	@${AR} x orig/slamWii.a AmcExi2Stubs.o --output obj/release/amcExi
	@${AR} x orig/slamWii.a odenotstub.o --output obj/release/odenotstub
	@${AR} x orig/slamWii.a TPL.o --output obj/release/tpl
	@${AR} x orig/slamWii.a THPDec.o THPStats.o THPAudio.o --output obj/release/thp
	@${AR} x orig/slamWii.a HBMFrameController.o HBMAnmController.o HBMGUIManager.o HBMController.o HBMRemoteSpk.o HBMAxSound.o HBMCommon.o HBMBase.o lyt_animation.o lyt_arcResourceAccessor.o lyt_bounding.o lyt_common.o lyt_drawInfo.o lyt_group.o lyt_init.o lyt_layout.o lyt_material.o lyt_pane.o lyt_picture.o lyt_resourceAccessor.o lyt_textBox.o lyt_window.o math_arithmetic.o math_equation.o math_geometry.o math_triangular.o math_types.o ut_binaryFileFormat.o ut_CharStrmReader.o ut_CharWriter.o ut_Font.o ut_LinkList.o ut_list.o ut_ResFont.o ut_ResFontBase.o ut_RomFont.o ut_TagProcessorBase.o ut_TextWriterBase.o --output obj/release/homebuttonLib
	@${AR} xN 2 orig/slamWii.a mix.o syn.o synctrl.o synenv.o synmix.o synpitch.o synsample.o synvoice.o seq.o --output obj/release/homebuttonLib
	@${AR} x orig/slamWii.a dvdfs.o dvd.o dvdqueue.o dvderror.o dvdidutils.o dvdFatal.o dvdDeviceError.o dvd_broadway.o --output obj/release/dvd
	@${FIND} obj/release -type f -exec chmod +r {} \;
endif

ifneq ($(wildcard asm/release),)
disasm-release:; $(info Release libraries already extracted.)
else
disasm-release: extract-release
	@$(foreach f,$(wildcard obj/release/*/*.o),dtk -L error elf disasm $f ${f:obj/%.o=asm/%.s};)
endif
