# archive link order

lib_list				= ai amcExi arc ax axart axfx base bte cx darch db dsp dvd enc esp euart exi fs gx hid homebuttonLib ipc kbd kpad mem midi mix mtx nand NdevExi2A odenotstub os pad rso sc seq si sp syn thp tpl usb vi wenc wpad

ai_lib_src				= ai.c
amcExi_lib_src			= AmcExi2Stubs.c
arc_lib_src				= arc.c
ax_lib_src				= AX.c AXAlloc.c AXAux.c AXCL.c AXOut.c AXSPB.c AXVPB.c AXProf.c AXComp.c DSPCode.c DSPADPCM.c
axart_lib_src			= axart.c axartsound.c axartcents.c axartenv.c axartlfo.c axart3d.c axartlpf.c
axfx_lib_src			= AXFXReverbHi.c AXFXReverbHiDpl2.c AXFXReverbHiExp.c AXFXReverbHiExpDpl2.c AXFXDelay.c AXFXDelayDpl2.c AXFXDelayExp.c AXFXDelayExpDpl2.c AXFXReverbStd.c AXFXReverbStdDpl2.c AXFXReverbStdExp.c AXFXReverbStdExpDpl2.c AXFXChorus.c AXFXChorusDpl2.c AXFXChorusExp.c AXFXChorusExpDpl2.c AXFXLfoTable.c AXFXSrcCoef.c AXFXHooks.c
base_lib_src			= PPCArch.c PPCPm.c
bte_lib_src				= gki_buffer.c gki_debug.c gki_time.c gki_ppc.c hcisu_h2.c uusb_ppc.c bta_aa_cfg.c bta_ag_cfg.c bta_av_cfg.c bta_bi_cfg.c bta_cg_cfg.c bta_ct_cfg.c bta_dg_cfg.c bta_dm_cfg.c bta_fs_cfg.c bta_ft_cfg.c bta_hd_cfg.c bta_hh_cfg.c bta_pbc_cfg.c bta_pbs_cfg.c bta_pr_cfg.c bta_ss_cfg.c bta_sys_cfg.c bte_disp.c bte_hcisu.c bte_init.c bte_load.c bte_logmsg.c bte_main.c bte_version.c btu_task1.c bd.c bta_sys_conn.c bta_sys_main.c ptim.c utl.c bta_prm_api.c bta_prm_main.c bta_fs_ci.c bta_dm_act.c bta_dm_api.c bta_dm_main.c bta_dm_pm.c bta_hd_act.c bta_hd_api.c bta_hd_main.c bta_hh_act.c bta_hh_api.c bta_hh_main.c bta_hh_utils.c btm_acl.c btm_dev.c btm_devctl.c btm_discovery.c btm_inq.c btm_main.c btm_pm.c btm_sco.c btm_sec.c btu_hcif.c btu_init.c wbt_ext.c gap_api.c gap_conn.c gap_utils.c goep_trace.c goep_util.c goep_fs.c hcicmds.c hidd_api.c hidd_conn.c hidd_mgmt.c hidd_pm.c hidh_api.c hidh_conn.c l2c_api.c l2c_csm.c l2c_link.c l2c_main.c l2c_utils.c port_api.c port_rfc.c port_utils.c rfc_l2cap_if.c rfc_mx_fsm.c rfc_port_fsm.c rfc_port_if.c rfc_ts_frames.c rfc_utils.c sdp_api.c sdp_db.c sdp_discovery.c sdp_main.c sdp_server.c sdp_utils.c xml_bld.c xml_parse.c
cx_lib_src				= CXCompression.c CXStreamingUncompression.c CXUncompression.c CXSecureUncompression.c
darch_lib_src			= darch.c
db_lib_src				= db.c
dsp_lib_src				= dsp.c dsp_debug.c dsp_perf.c dsp_task.c
dvd_lib_src				= dvdfs.c dvd.c dvdqueue.c dvderror.c dvdidutils.c dvdFatal.c dvdDeviceError.c dvd_broadway.c
enc_lib_src				= encutility.c encunicode.c encjapanese.c enclatin.c encconvert.c encchinese.c enckorean.c
esp_lib_src				= esp.c
euart_lib_src			= euart.c
exi_lib_src				= EXIBios.c EXIUart.c EXIAd16.c EXICommon.c
fs_lib_src				= fs.c
gx_lib_src				= GXInit.c GXFifo.c GXAttr.c GXMisc.c GXGeometry.c GXFrameBuf.c GXLight.c GXTexture.c GXBump.c GXTev.c GXPixel.c GXDraw.c GXDisplayList.c GXVert.c GXTransform.c GXVerify.c GXVerifXF.c GXVerifRAS.c GXSave.c GXPerf.c
hid_lib_src				= hid_api.c hid_ios.c hid_client.c hid_device.c hid_interface.c hid_open_close.c hid_task.c
homebuttonLib_lib_src	= HBMFrameController.c HBMAnmController.c HBMGUIManager.c HBMController.c HBMRemoteSpk.c HBMAxSound.c HBMCommon.c HBMBase.c lyt_animation.c lyt_arcResourceAccessor.c lyt_bounding.c lyt_common.c lyt_drawInfo.c lyt_group.c lyt_init.c lyt_layout.c lyt_material.c lyt_pane.c lyt_picture.c lyt_resourceAccessor.c lyt_textBox.c lyt_window.c math_arithmetic.c math_equation.c math_geometry.c math_triangular.c math_types.c ut_binaryFileFormat.c ut_CharStrmReader.c ut_CharWriter.c ut_Font.c ut_LinkList.c ut_list.c ut_ResFont.c ut_ResFontBase.c ut_RomFont.c ut_TagProcessorBase.c ut_TextWriterBase.c mix.c syn.c synctrl.c synenv.c synmix.c synpitch.c synsample.c synvoice.c seq.c
ipc_lib_src				= ipcMain.c ipcclt.c memory.c ipcProfile.c
kbd_lib_src				= kbd_lib.c kbd_lib_led.c kbd_lib_init.c kbd_lib_maps_us.c kbd_lib_maps_jp.c kbd_lib_maps_eu.c
kpad_lib_src			= KPAD.c KMPLS.c KZMplsTestSub.c
mem_lib_src				= mem_heapCommon.c mem_expHeap.c mem_frameHeap.c mem_unitHeap.c mem_allocator.c mem_list.c
midi_lib_src			= MIDI.c MIDIRead.c MIDIXfer.c
mix_lib_src				= mix.c remote.c
mtx_lib_src				= mtx.c mtxvec.c mtxstack.c mtx44.c mtx44vec.c vec.c quat.c psmtx.c
nand_lib_src			= nand.c NANDOpenClose.c NANDCore.c NANDSecret.c NANDCheck.c NANDLogging.c NANDErrorMessage.c
NdevExi2A_lib_src		= DebuggerDriver.c exi2.c
odenotstub_lib_src		= odenotstub.c
os_lib_src				= OS.c OSAddress.c OSAlarm.c OSAlloc.c OSArena.c OSAudioSystem.c OSCache.c OSContext.c OSError.c OSExec.c OSFatal.c OSFont.c OSInterrupt.c OSLink.c OSMessage.c OSMemory.c OSMutex.c OSReboot.c OSReset.c OSRtc.c OSSemaphore.c OSStopwatch.c OSSync.c OSThread.c OSTime.c OSTimer.c OSUtf.c OSIpc.c OSStateTM.c __start.c OSPlayRecord.c OSStateFlags.c OSNet.c OSNandbootInfo.c OSPlayTime.c OSInstall.c OSCrc.c OSLaunch.c __ppc_eabi_init.c
pad_lib_src				= Padclamp.c Pad.c
rso_lib_src				= RSOLink.c
sc_lib_src				= scsystem.c scapi.c scapi_net.c scapi_prdinfo.c
seq_lib_src				= seq.c
si_lib_src				= SIBios.c SISamplingRate.c SISteering.c SISteeringXfer.c SISteeringAuto.c
sp_lib_src				= sp.c
syn_lib_src				= syn.c synctrl.c synenv.c synlfo.c synmix.c synpitch.c synsample.c synvoice.c synwt.c
thp_lib_src				= THPDec.c THPStats.c THPAudio.c
tpl_lib_src				= TPL.c
usb_lib_src				= usb.c
vi_lib_src				= vi.c i2c.c vi3in1.c
wenc_lib_src			= wenc.c
wpad_lib_src			= WPAD.c WPADHIDParser.c WPADEncrypt.c WPADClamp.c WPADMem.c lint.c WUD.c WUDHidHost.c

define define_lib =
lib/$1D.a: $$(addprefix build/debug/$1/,$${$2:.c=.o})
lib/$1.a: $$(addprefix build/release/$1/,$${$2:.c=.o})
endef

$(foreach l,${lib_list},$(eval $(call define_lib,$l,$l_lib_src)))

define make_deps =
depends += $$(addprefix build/deps/debug/$1/,$${$2:.c=.d})
depends += $$(addprefix build/deps/release/$1/,$${$2:.c=.d})
endef

$(foreach l,${lib_list},$(eval $(call make_deps,$l,$l_lib_src)))

-include ${depends}
