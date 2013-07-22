/*
 * OMAP4 OPP table definitions.
 *
 * Copyright (C) 2010-2011 Texas Instruments Incorporated - http://www.ti.com/
 *	Nishanth Menon
 *	Kevin Hilman
 *	Thara Gopinath
 * Copyright (C) 2010-2011 Nokia Corporation.
 *      Eduardo Valentin
 *      Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/opp.h>

#include <plat/cpu.h>
#include <plat/common.h>

#include "control.h"
#include "omap_opp_data.h"
#include "pm.h"

/*
 * STD_FUSE_OPP_DPLL_1 contains info about ABB trim type for MPU/IVA.
 * This bit field definition is specific for OMAP4460 TURBO alone.
 * For future OMAP4 silicon it is possible that other efuse offsets might
 * be used in addition to controlling other OPPs as well.
 * This probably is an ugly location to put the DPLL trim details.. but,
 * alternatives are even less attractive :( shrug..
 */
#define OMAP4460_MPU_OPP_DPLL_TRIM     BIT(18)
#define OMAP4460_MPU_OPP_DPLL_TURBO_RBB        BIT(20)

/*
 * Structures containing OMAP4430 voltage supported and various
 * voltage dependent data for each VDD.
 */

#define OMAP4430_VDD_MPU_OPP50_UV		1025000
#define OMAP4430_VDD_MPU_OPP100_UV		1200000
#define OMAP4430_VDD_MPU_OPPTURBO_UV		1325000
#define OMAP4430_VDD_MPU_OPPNITRO_UV		1388000

struct omap_volt_data omap443x_vdd_mpu_volt_data[] = {
	VOLT_DATA_DEFINE(OMAP4430_VDD_MPU_OPP50_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4430_VDD_MPU_OPP100_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPP100, 0xf9, 0x16, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4430_VDD_MPU_OPPTURBO_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPPTURBO, 0xfa, 0x23, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4430_VDD_MPU_OPPNITRO_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPPNITRO, 0xfa, 0x27, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

#define OMAP4430_VDD_IVA_OPP50_UV		 950000
#define OMAP4430_VDD_IVA_OPP100_UV		1114000
#define OMAP4430_VDD_IVA_OPPTURBO_UV		1291000

struct omap_volt_data omap443x_vdd_iva_volt_data[] = {
	VOLT_DATA_DEFINE(OMAP4430_VDD_IVA_OPP50_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4430_VDD_IVA_OPP100_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPP100, 0xf9, 0x16, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4430_VDD_IVA_OPPTURBO_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPPTURBO, 0xfa, 0x23, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

#define OMAP4430_VDD_CORE_OPP50_UV		 962000
#define OMAP4430_VDD_CORE_OPP100_UV		1127000

struct omap_volt_data omap443x_vdd_core_volt_data[] = {
	VOLT_DATA_DEFINE(OMAP4430_VDD_CORE_OPP50_UV, 0, OMAP44XX_CONTROL_FUSE_CORE_OPP50, 0xf4, 0x0c, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(OMAP4430_VDD_CORE_OPP100_UV, 0, OMAP44XX_CONTROL_FUSE_CORE_OPP100, 0xf9, 0x16, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

/* Dependency of domains are as follows for OMAP4430 (OPP based):
 *
 *	MPU	IVA	CORE
 *	50	50	50+
 *	50	100+	100
 *	100+	50	100
 *	100+	100+	100
 */

/* OMAP 4430 MPU Core VDD dependency table */
static struct omap_vdd_dep_volt omap443x_vdd_mpu_core_dep_data[] = {
	{.main_vdd_volt = OMAP4430_VDD_MPU_OPP50_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4430_VDD_MPU_OPP100_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP100_UV},
	{.main_vdd_volt = OMAP4430_VDD_MPU_OPPTURBO_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP100_UV},
	{.main_vdd_volt = OMAP4430_VDD_MPU_OPPNITRO_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP100_UV},
};

struct omap_vdd_dep_info omap443x_vddmpu_dep_info[] = {
	{
		.name	= "core",
		.dep_table = omap443x_vdd_mpu_core_dep_data,
		.nr_dep_entries = ARRAY_SIZE(omap443x_vdd_mpu_core_dep_data),
	},
	{.name = NULL, .dep_table = NULL, .nr_dep_entries = 0},
};

/* OMAP 4430 MPU IVA VDD dependency table */
static struct omap_vdd_dep_volt omap443x_vdd_iva_core_dep_data[] = {
	{.main_vdd_volt = OMAP4430_VDD_IVA_OPP50_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4430_VDD_IVA_OPP100_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP100_UV},
	{.main_vdd_volt = OMAP4430_VDD_IVA_OPPTURBO_UV, .dep_vdd_volt = OMAP4430_VDD_CORE_OPP100_UV},
};

struct omap_vdd_dep_info omap443x_vddiva_dep_info[] = {
	{
		.name	= "core",
		.dep_table = omap443x_vdd_iva_core_dep_data,
		.nr_dep_entries = ARRAY_SIZE(omap443x_vdd_iva_core_dep_data),
	},
	{.name = NULL, .dep_table = NULL, .nr_dep_entries = 0},
};

static struct omap_opp_def __initdata omap443x_opp_def_list[] = {
	/* MPU OPP1 - OPP50 */
	OPP_INITIALIZER("mpu", "dpll_mpu_ck", "mpu", true, 300000000, OMAP4430_VDD_MPU_OPP50_UV),
	/* MPU OPP2 - OPP100 */
	OPP_INITIALIZER("mpu", "dpll_mpu_ck", "mpu", true, 600000000, OMAP4430_VDD_MPU_OPP100_UV),
	/* MPU OPP3 - OPP-Turbo */
	OPP_INITIALIZER("mpu", "dpll_mpu_ck", "mpu", true, 800000000, OMAP4430_VDD_MPU_OPPTURBO_UV),
	/* MPU OPP4 - OPP-SB */
	OPP_INITIALIZER("mpu", "dpll_mpu_ck", "mpu", true, 1008000000, OMAP4430_VDD_MPU_OPPNITRO_UV),
	/* L3 OPP1 - OPP50 */
	OPP_INITIALIZER("l3_main_1", "virt_l3_ck", "core", true, 100000000, OMAP4430_VDD_CORE_OPP50_UV),
	/* L3 OPP2 - OPP100, OPP-Turbo, OPP-SB */
	OPP_INITIALIZER("l3_main_1", "virt_l3_ck", "core", true, 200000000, OMAP4430_VDD_CORE_OPP100_UV),
	/* IVA OPP1 - OPP50 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", true, 133000000, OMAP4430_VDD_IVA_OPP50_UV),
	/* IVA OPP2 - OPP100 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", true, 266100000, OMAP4430_VDD_IVA_OPP100_UV),
	/* IVA OPP3 - OPP-Turbo */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", false, 332000000, OMAP4430_VDD_IVA_OPPTURBO_UV),
	/* SGX OPP1 - OPP50 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", false, 332000000, OMAP4430_VDD_IVA_OPPTURBO_UV),
	/* SGX OPP2 - OPP100 */
	OPP_INITIALIZER("gpu", "dpll_per_m7x2_ck", "core", true, 307200000, OMAP4430_VDD_CORE_OPP100_UV),
	/* FDIF OPP1 - OPP25 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 32000000, OMAP4430_VDD_CORE_OPP50_UV),
	/* FDIF OPP2 - OPP50 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 64000000, OMAP4430_VDD_CORE_OPP50_UV),
	/* FDIF OPP3 - OPP100 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 128000000, OMAP4430_VDD_CORE_OPP100_UV),
	/* DSP OPP1 - OPP50 */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", true, 232750000, OMAP4430_VDD_IVA_OPP50_UV),
	/* DSP OPP2 - OPP100 */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", true, 465500000, OMAP4430_VDD_IVA_OPP100_UV),
	/* DSP OPP3 - OPPTB */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", false, 496000000, OMAP4430_VDD_IVA_OPPTURBO_UV),
	/* HSI OPP1 - OPP50 */
	OPP_INITIALIZER("hsi", "hsi_fck", "core", true, 96000000, OMAP4430_VDD_CORE_OPP50_UV),
	/* HSI OPP2 - OPP100 */
	OPP_INITIALIZER("hsi", "hsi_fck", "core", true, 96000000, OMAP4430_VDD_CORE_OPP100_UV),
	/* ABE OPP1 - OPP50 */
	OPP_INITIALIZER("aess", "abe_clk", "iva", true, 98304000, OMAP4430_VDD_IVA_OPP50_UV),
	/* ABE OPP2 - OPP100 */
	OPP_INITIALIZER("aess", "abe_clk", "iva", true, 196608000, OMAP4430_VDD_IVA_OPP100_UV),
	/* DSS OPP1 - OPP50 */
	OPP_INITIALIZER("dss_dispc", "virt_lcd_pclk", "core", true, 93000000, OMAP4430_VDD_CORE_OPP50_UV),
	/* DSS OPP2 - OPP100 */
	OPP_INITIALIZER("dss_dispc", "virt_lcd_pclk", "core", true, 170000000, OMAP4430_VDD_CORE_OPP100_UV),
};

#define OMAP4460_VDD_MPU_OPP25_UV		 850000
#define OMAP4460_VDD_MPU_OPP50_UV		 870000
#define OMAP4460_VDD_MPU_OPP75_UV		 950000
#define OMAP4460_VDD_MPU_OPP100_UV		1040000
#define OMAP4460_VDD_MPU_OPPTURBO_UV		1170000
#define OMAP4460_VDD_MPU_OPPNITRO_UV		1250000
#define OMAP4460_VDD_MPU_OPP_1_UV		1300000
#define OMAP4460_VDD_MPU_OPP_2_UV		1340000
#define OMAP4460_VDD_MPU_OPP_3_UV		1380000

struct omap_volt_data omap446x_vdd_mpu_volt_data[] = {
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP25_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP50_UV, 10000, OMAP44XX_CONTROL_FUSE_MPU_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP75_UV, 80000, OMAP44XX_CONTROL_FUSE_MPU_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP100_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPP100, 0xf9, 0x16, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPPTURBO_UV, 10000, OMAP44XX_CONTROL_FUSE_MPU_OPPTURBO, 0xfa, 0x23, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPPNITRO_UV, 0, OMAP44XX_CONTROL_FUSE_MPU_OPPNITRO, 0xfa, 0x27, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP_1_UV, 40000, OMAP44XX_CONTROL_FUSE_MPU_OPPNITRO, 0xfa, 0x27, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP_2_UV, 80000, OMAP44XX_CONTROL_FUSE_MPU_OPPNITRO, 0xfa, 0x27, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_MPU_OPP_3_UV, 150000, OMAP44XX_CONTROL_FUSE_MPU_OPPNITRO, 0xfa, 0x27, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

#define OMAP4460_VDD_IVA_OPP50_UV		 890000
#define OMAP4460_VDD_IVA_OPP100_UV		1000000
#define OMAP4460_VDD_IVA_OPPTURBO_UV		1291000
#define OMAP4460_VDD_IVA_OPPNITRO_UV		1375000

struct omap_volt_data omap446x_vdd_iva_volt_data[] = {
	VOLT_DATA_DEFINE(OMAP4460_VDD_IVA_OPP50_UV, 13000, OMAP44XX_CONTROL_FUSE_IVA_OPP50, 0xf4, 0x0c, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_IVA_OPP100_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPP100, 0xf9, 0x16, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_IVA_OPPTURBO_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPPTURBO, 0xfa, 0x23, OMAP_ABB_NOMINAL_OPP),
	VOLT_DATA_DEFINE(OMAP4460_VDD_IVA_OPPNITRO_UV, 0, OMAP44XX_CONTROL_FUSE_IVA_OPPNITRO, 0xfa, 0x23, OMAP_ABB_FAST_OPP),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

#define OMAP4460_VDD_CORE_OPP50_UV		 910000
#define OMAP4460_VDD_CORE_OPP75_UV		1050000
#define OMAP4460_VDD_CORE_OPP100_UV		1075000
#define OMAP4460_VDD_CORE_OPP100_OV_UV		1120000

struct omap_volt_data omap446x_vdd_core_volt_data[] = {
	
	VOLT_DATA_DEFINE(OMAP4460_VDD_CORE_OPP50_UV, 38000, OMAP44XX_CONTROL_FUSE_CORE_OPP50, 0xf4, 0x0c, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(OMAP4460_VDD_CORE_OPP75_UV, 13000, OMAP44XX_CONTROL_FUSE_CORE_OPP100, 0xf9, 0x16, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(OMAP4460_VDD_CORE_OPP100_UV, 13000, OMAP44XX_CONTROL_FUSE_CORE_OPP100OV, 0xf9, 0x16, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(OMAP4460_VDD_CORE_OPP100_OV_UV, 75000, OMAP44XX_CONTROL_FUSE_CORE_OPP100OV, 0xf9, 0x16, OMAP_ABB_NONE),
	VOLT_DATA_DEFINE(0, 0, 0, 0, 0, 0),
};

/* OMAP 4460 MPU Core VDD dependency table */
static struct omap_vdd_dep_volt omap446x_vdd_mpu_core_dep_data[] = {
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP25_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP50_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP75_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP100_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPPTURBO_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPPNITRO_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP_1_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP_2_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_MPU_OPP_3_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
};

struct omap_vdd_dep_info omap446x_vddmpu_dep_info[] = {
	{
		.name	= "core",
		.dep_table = omap446x_vdd_mpu_core_dep_data,
		.nr_dep_entries = ARRAY_SIZE(omap446x_vdd_mpu_core_dep_data),
	},
	{.name = NULL, .dep_table = NULL, .nr_dep_entries = 0},
};

/* OMAP 4460 MPU IVA VDD dependency table */
static struct omap_vdd_dep_volt omap446x_vdd_iva_core_dep_data[] = {
	{.main_vdd_volt = OMAP4460_VDD_IVA_OPP50_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP50_UV},
	{.main_vdd_volt = OMAP4460_VDD_IVA_OPP100_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_IVA_OPPTURBO_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
	{.main_vdd_volt = OMAP4460_VDD_IVA_OPPNITRO_UV, .dep_vdd_volt = OMAP4460_VDD_CORE_OPP75_UV},
};

struct omap_vdd_dep_info omap446x_vddiva_dep_info[] = {
	{
		.name	= "core",
		.dep_table = omap446x_vdd_iva_core_dep_data,
		.nr_dep_entries = ARRAY_SIZE(omap446x_vdd_iva_core_dep_data),
	},
	{.name = NULL, .dep_table = NULL, .nr_dep_entries = 0},
};

static struct omap_opp_def __initdata omap446x_opp_def_list[] = {
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", true, 192000000, OMAP4460_VDD_MPU_OPP25_UV),
	/* MPU OPP1 - OPP50 */
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", true, 384000000, OMAP4460_VDD_MPU_OPP50_UV),
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", true, 537600000, OMAP4460_VDD_MPU_OPP75_UV),
	/* MPU OPP2 - OPP100 */
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", true, 729600000, OMAP4460_VDD_MPU_OPP100_UV),
	/* MPU OPP3 - OPP-Turbo */
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", true, 1036800000, OMAP4460_VDD_MPU_OPPTURBO_UV),
	/* MPU OPP4 - OPP-Nitro */
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", false, 1228800000, OMAP4460_VDD_MPU_OPPNITRO_UV),
	/* MPU OPP4 - OPP-Nitro SpeedBin */
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", false, 1344000000, OMAP4460_VDD_MPU_OPP_1_UV),
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", false, 1420800000, OMAP4460_VDD_MPU_OPP_2_UV),
	OPP_INITIALIZER("mpu", "virt_dpll_mpu_ck", "mpu", false, 1536000000, OMAP4460_VDD_MPU_OPP_3_UV),
	/* L3 OPP1 - OPP50 */
	OPP_INITIALIZER("l3_main_1", "virt_l3_ck", "core", true, 100000000, OMAP4460_VDD_CORE_OPP50_UV),
	/* L3 OPP2 - OPP100 */
	OPP_INITIALIZER("l3_main_1", "virt_l3_ck", "core", true, 200000000, OMAP4460_VDD_CORE_OPP75_UV),
	OPP_INITIALIZER("l3_main_1", "virt_l3_ck", "core", true, 200000000, OMAP4460_VDD_CORE_OPP100_UV),
	/* IVA OPP1 - OPP50 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", true, 133000000, OMAP4460_VDD_IVA_OPP50_UV),
	/* IVA OPP2 - OPP100 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", true, 266100000, OMAP4460_VDD_IVA_OPP100_UV),
	/*
	 * IVA OPP3 - OPP-Turbo + Disabled as the reference schematics
	 * recommends Phoenix VCORE2 which can supply only 600mA - so the ones
	 * above this OPP frequency, even though OMAP is capable, should be
	 * enabled by board file which is sure of the chip power capability
	 */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", false, 332000000, OMAP4460_VDD_IVA_OPPTURBO_UV),
	/* IVA OPP4 - OPP-Nitro */
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", false, 430000000, OMAP4460_VDD_IVA_OPPNITRO_UV),
	/* IVA OPP5 - OPP-Nitro SpeedBin*/
	OPP_INITIALIZER("iva", "virt_iva_ck", "iva", false, 500000000, OMAP4460_VDD_IVA_OPPNITRO_UV),

	/* SGX OPP1 - OPP50 */
	OPP_INITIALIZER("gpu", "dpll_per_m7x2_ck", "core", true, 153600000, OMAP4460_VDD_CORE_OPP50_UV),
	/* SGX OPP2 - OPP100 */
	OPP_INITIALIZER("gpu", "dpll_per_m7x2_ck", "core", false, 307200000, OMAP4460_VDD_CORE_OPP75_UV),
	/* SGX OPP3 - OPPOV */
	OPP_INITIALIZER("gpu", "dpll_per_m7x2_ck", "core", true, 384000000, OMAP4460_VDD_CORE_OPP100_UV),
	/* SGX OPP3 - OPPOV */
	OPP_INITIALIZER("gpu", "dpll_per_m7x2_ck", "core", false, 512000000, OMAP4460_VDD_CORE_OPP100_OV_UV),
	/* FDIF OPP1 - OPP25 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 32000000, OMAP4460_VDD_CORE_OPP50_UV),
	/* FDIF OPP2 - OPP50 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 64000000, OMAP4460_VDD_CORE_OPP50_UV),
	/* FDIF OPP3 - OPP100 */
	OPP_INITIALIZER("fdif", "fdif_fck", "core", true, 128000000, OMAP4460_VDD_CORE_OPP75_UV),
	/* DSP OPP1 - OPP50 */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", true, 232750000, OMAP4460_VDD_IVA_OPP50_UV),
	/* DSP OPP2 - OPP100 */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", true, 465500000, OMAP4460_VDD_IVA_OPP100_UV),
	/* DSP OPP3 - OPPTB */
	OPP_INITIALIZER("dsp", "virt_dsp_ck", "iva", false, 496000000, OMAP4460_VDD_IVA_OPPTURBO_UV),
	/* HSI OPP1 - OPP50 */
	OPP_INITIALIZER("hsi", "hsi_fck", "core", true, 96000000, OMAP4460_VDD_CORE_OPP50_UV),
	/* HSI OPP2 - OPP100 */
	OPP_INITIALIZER("hsi", "hsi_fck", "core", true, 96000000, OMAP4460_VDD_CORE_OPP75_UV),
	/* ABE OPP1 - OPP50 */
	OPP_INITIALIZER("aess", "abe_clk", "iva", true, 98304000, OMAP4460_VDD_IVA_OPP50_UV),
	/* ABE OPP2 - OPP100 */
	OPP_INITIALIZER("aess", "abe_clk", "iva", true, 196608000, OMAP4460_VDD_IVA_OPP100_UV),
	/* DSS OPP1 - OPP50 */
	OPP_INITIALIZER("dss_dispc", "virt_lcd_pclk", "core", true, 93000000, OMAP4460_VDD_CORE_OPP50_UV),
	/* DSS OPP2 - OPP100 */
	OPP_INITIALIZER("dss_dispc", "virt_lcd_pclk", "core", true, 170000000, OMAP4460_VDD_CORE_OPP75_UV),
};

/**
 * omap4_mpu_opp_enable() - helper to enable the OPP
 * @freq:	frequency to enable
 */
static void __init omap4_mpu_opp_enable(unsigned long freq)
{
	struct device *mpu_dev;
	int r;

	mpu_dev = omap2_get_mpuss_device();
	if (!mpu_dev) {
		pr_err("%s: no mpu_dev, did not enable f=%ld\n", __func__,
			freq);
		return;
	}

	r = opp_enable(mpu_dev, freq);
	if (r < 0)
		dev_err(mpu_dev, "%s: opp_enable failed(%d) f=%ld\n", __func__,
			r, freq);
}

/**
 * omap4_abb_update() - update the ABB map for a specific voltage in table
 * @vtable:    voltage table to update
 * @voltage:   voltage whose voltage data needs update
 * @abb_type:  what ABB type should we update it to?
 */
static void __init omap4_abb_update(struct omap_volt_data *vtable,
                                   unsigned long voltage, int abb_type)
{
	/* scan through and update the voltage table */
	while (vtable->volt_nominal) {
		if (vtable->volt_nominal == voltage) {
			vtable->abb_type = abb_type;
			return;
		}
		vtable++;
	}
	/* WARN noticably to get the developer to fix */
	WARN(1, "%s: voltage %ld could not be set to ABB %d\n",
		__func__, voltage, abb_type);
}

/**
 * omap4460_abb_update() - update the abb mapping quirks for OMAP4460
 */
static void __init omap4460_abb_update(void)
{
	u32 reg, trim, rbb;
	char *abb_msg;
	int abb_type;

	/*
	 * Determine MPU ABB state at OPP_TURBO on 4460
	 *
	 * On 4460 all OPPs have preset states for the MPU's ABB LDO, except
	 * for OPP_TURBO.  OPP_TURBO may require bypass, FBB or RBB depending
	 * on a combination of characterisation data blown into eFuse register
	 * CONTROL_STD_FUSE_OPP_DPLL_1.
	 *
	 * Bits 18 & 19 of that register signify DPLL_MPU trim (see
	 * arch/arm/mach-omap2/omap4-trim-quirks.c). OPP_TURBO might put MPU's
	 * ABB LDO into bypass or FBB based on this value.
	 *
	 * Bit 20 siginifies if RBB should be enabled. If set it will always
	 * override the values from bits 18 & 19.
	 *
	 * The table below captures the valid combinations:
	 * trim          rbb
	 * Bit 18|Bit 19|Bit 20|ABB type
	 * 0      0      0      bypass
	 * 0      1      0      bypass  (invalid combination)
	 * 1      0      0      FBB     (2.4GHz DPLL_MPU)
	 * 1      1      0      FBB     (3GHz DPLL_MPU)
	 * 0      0      1      RBB
	 * 0      1      1      RBB     (invalid combination)
	 * 1      0      1      RBB     (2.4GHz DPLL_MPU)
	 * 1      1      1      RBB     (3GHz DPLL_MPU)
	 */
	reg = omap_ctrl_readl(OMAP4_CTRL_MODULE_CORE_STD_FUSE_OPP_DPLL_1);
	trim = reg & OMAP4460_MPU_OPP_DPLL_TRIM;
	rbb = reg & OMAP4460_MPU_OPP_DPLL_TURBO_RBB;

	if (rbb) {
		abb_type = OMAP_ABB_SLOW_OPP;
		abb_msg = "Slow";
	} else if (!trim) {
		abb_type = OMAP_ABB_NOMINAL_OPP;
		abb_msg = "un-trimmed Nominal";
	} else {
		abb_type = OMAP_ABB_FAST_OPP;
		abb_msg = "Fast";
	}

	omap4_abb_update(omap446x_vdd_mpu_volt_data,
			OMAP4460_VDD_MPU_OPPTURBO_UV,
			abb_type);
	pr_info("%s: MPU OPP Turbo: %s OPP ABB type\n", __func__, abb_msg);
}

/**
 * omap4_opp_init() - initialize omap4 opp table
 */
int __init omap4_opp_init(void)
{
	int r = -ENODEV;

	r = omap_init_opp_table(omap446x_opp_def_list,
		ARRAY_SIZE(omap446x_opp_def_list));

	omap4_mpu_opp_enable(1228800000);
	omap4_mpu_opp_enable(1344000000);
	omap4_mpu_opp_enable(1420800000);
#ifdef CONFIG_TUNA_1_5_GHZ_MPU_OPP
	omap4_mpu_opp_enable(1536000000);
#endif

	/* Update ABB settings */
	if (cpu_is_omap446x())
		omap4460_abb_update();

	return r;
}
device_initcall(omap4_opp_init);
