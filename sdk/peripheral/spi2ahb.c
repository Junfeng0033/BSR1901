/****************************************************************************
 *   $Id:: spi2ahb.c 2025-07-06 22:40:16
 *   Project: BSR SPI2AHB driver
 *
 *   Description:
 *     This file contains SPI2AHB code example
 *   Author:
 *   	 GF
****************************************************************************/


#include "platform_config.h"




//====================================================================================================
//
// SPI Slave Write/Read command format
//
//====================================================================================================

//====================================================================================================
//
// SPI Slave Write/Read command format
//
//====================================================================================================







/**********************************************************
*
*#define GECKO_AON_BASE_ADDR     0x4001A000
*
*Oscillator Calibration:
*0x20 default 0x808E7885:
*05 40 01 A0 24 00 00 00 10
*05 40 01 A0 20 80 8E 78 85
*
*
***********************************************************/



/*


//==============================================================
//  Created @ 2023.01.24
//  TestCase of SPI-to-AHB debug port
//==============================================================

localparam TEST_DATA0 = 32'h1122_3344;
localparam TEST_DATA1 = 32'h5566_7788;
localparam TEST_DATA2 = 32'h99AA_BBCC;
localparam TEST_DATA3 = 32'hDDEE_F0F0;

reg  [8*256-1:0]    tc_msg;
reg  [7:0]          tc_spi_cmd;
reg  [7:0]          tc_spi_rdata;
reg  [31:0]         tc_spi2ahb_addr;
reg  [31:0]         tc_spi2ahb_wdat;
reg  [31:0]         tc_spi2ahb_rdat;

assign PAD_08 = spi_drv_sclk;
assign PAD_09 = spi_drv_mosi;
assign spi_drv_miso = PAD_10;
assign PAD_11 = spi_drv_scsn;

initial begin : TC_SPI2AHB_TEST_SEQUENCE
    tc_msg="TC_Start";
    spi_drv_scsn_ctrl = 1'h1;
    spi_drv_wr_req    = 1'h0;
    spi_drv_data_i    = 8'h0;
    wait(`DUT_DIGITAL_BUS_RSTN==1'b1); #10us;
    $display("[TC Message] Start TestCase.");

    //  Directly Read ChipID
    //  Expect read-back: 0x5441_524B
    //  T - ASCII: 0x54
    //  A - ASCII: 0x41
    //  R - ASCII: 0x52
    //  K - ASCII: 0x4B
    tc_msg="TC_01";
    tc_spi2ahb_addr = `ADDR_BASE_APB_SYSCTRL;
    spi_task_rd_ahb_directly(tc_spi2ahb_addr, tc_spi2ahb_rdat); // ahb_addr[31:0]; ahb_rdata[31:0];
    $display("[TC Message] SPI Read data (32bit): %x", tc_spi2ahb_rdat); tc_msg="Read_AHB_Directly";

    //  Write and Read AHB register via SPI-to-AHB
    tc_msg="TC_02_Write_AHB";
    tc_spi2ahb_addr = `ADDR_BASE_APB_AON + 12'h040; // AON_RET_DATA0
    tc_spi2ahb_wdat = TEST_DATA0;
    spi_task_wr_ahb(tc_spi2ahb_addr, tc_spi2ahb_wdat); // ahb_addr[31:0]; ahb_wdata[31:0];

    tc_spi2ahb_addr = `ADDR_BASE_APB_AON + 12'h044; // AON_RET_DATA1
    tc_spi2ahb_wdat = TEST_DATA1;
    spi_task_wr_ahb(tc_spi2ahb_addr, tc_spi2ahb_wdat); // ahb_addr[31:0]; ahb_wdata[31:0];

    tc_msg="TC_02_Read_AHB_Directly";
    tc_spi2ahb_addr = `ADDR_BASE_APB_AON + 12'h040; // AON_RET_DATA0
    spi_task_rd_ahb_directly(tc_spi2ahb_addr, tc_spi2ahb_rdat); // ahb_addr[31:0]; ahb_rdata[31:0];
    $display("[TC Message] SPI Read data (32bit): %x. Expected: %x", tc_spi2ahb_rdat, TEST_DATA0);

    tc_spi2ahb_addr = `ADDR_BASE_APB_AON + 12'h044; // AON_RET_DATA0
    spi_task_rd_ahb_directly(tc_spi2ahb_addr, tc_spi2ahb_rdat); // ahb_addr[31:0]; ahb_rdata[31:0];
    $display("[TC Message] SPI Read data (32bit): %x. Expected: %x", tc_spi2ahb_rdat, TEST_DATA1);

    #100us;
    $display("[TC Message] A SoC sanity testcase is finished.");
    $finish;
end // TC_SPI2AHB_TEST_SEQUENCE



*/

