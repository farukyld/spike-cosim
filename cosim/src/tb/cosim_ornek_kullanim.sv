module cosim_ornek_kullanim;
  import cosim_pkg::*;

  commit_log_reg_item_t log_reg_write_from_c[CommitLogEntries];
  commit_log_mem_item_t log_mem_read_from_c[CommitLogEntries];
  commit_log_mem_item_t log_mem_write_from_c[CommitLogEntries];
  int num_elements_inserted_from_c_side; // 3'u icin de kullaniliyor.
  reg_t temp_key;
  freg_t temp_value;
  reg_t temp_pc;
  initial begin: cosimulation
    init();

    for (;;) begin: simulation_loop
      get_pc(temp_pc);
      $display("pc: %0h", temp_pc);
      if (simulation_completed() || temp_pc == 64'h8000_001c) begin // htif_t::exitcode != 0
        $display("simulation completed");
        break;
      end



      step();

      get_log_reg_write(log_reg_write_from_c, num_elements_inserted_from_c_side);

      for (int ii = 0; ii < num_elements_inserted_from_c_side; ii = ii + 1) begin: log_reg_write_itr

        $display("log_reg_write_from_c[%0d] reg_type: %0s", 
        ii, log_reg_write_from_c[ii].key.reg_type.name);

        if (log_reg_write_from_c[ii].key.reg_type == CSR) begin
          $display("log_reg_write_from_c[%0d] csr name: %0s",
          ii, log_reg_write_from_c[ii].key.reg_id.csr_id.name);
        end else begin
          $display("log_reg_write_from_c[%0d] reg_id: %0d", 
          ii, log_reg_write_from_c[ii].key.reg_id);
        end

        $display("log_reg_write_from_c[%0d].value: %0h", ii, log_reg_write_from_c[ii].value);
      end

      get_log_mem_read(log_mem_read_from_c, num_elements_inserted_from_c_side);

      for (int ii = 0; ii < num_elements_inserted_from_c_side; ii = ii + 1) begin: log_mem_read_itr
        $display("log_mem_read_from_c[%0d].addr: %0h", ii, log_mem_read_from_c[ii].addr);
        $display("log_mem_read_from_c[%0d].wdata: %0h", ii, log_mem_read_from_c[ii].wdata);
        $display("log_mem_read_from_c[%0d].len: %0d", ii, log_mem_read_from_c[ii].len);
      end

      get_log_mem_write(log_mem_write_from_c, num_elements_inserted_from_c_side);

      for (int ii = 0; ii < num_elements_inserted_from_c_side; ii = ii + 1) begin: log_mem_write_itr
        $display("log_mem_write_from_c[%0d].addr: %0h", ii, log_mem_write_from_c[ii].addr);
        $display("log_mem_write_from_c[%0d].wdata: %0h", ii, log_mem_write_from_c[ii].wdata);
        $display("log_mem_write_from_c[%0d].len: %0h", ii, log_mem_write_from_c[ii].len);
      end
      wait_key();

    end: simulation_loop
    $finish;
  end: cosimulation


// parameter int unsigned Steps = 11;
// commit_log_reg_item_t log_reg_writes_rtl[][] = 
// {
//   { // pc 0x1000
//     '{key:'{reg_type:XREG, reg_id:5}, value:128'h1000}
//   },
//   { // pc 0x1004
//     '{key:'{reg_type:XREG, reg_id:11}, value:128'h1020}
//   },
//   { // pc 0x1008
//     '{key:'{reg_type:XREG, reg_id:10}, value:128'h0}
//   },
//   { // pc 0x100c
//     '{key:'{reg_type:XREG, reg_id:5}, value:128'h80000004}
//   },
//   { // pc 1010
//     // jr instruction
//     '{key:'{reg_type:XREG, reg_id:0}, value:128'h1014}
//   },
//   { // pc 0x8000004
//     '{key:'{reg_type:XREG, reg_id:2}, value:128'h80009004}
//   },
//   { // pc 0x8000008
//     '{key:'{reg_type:XREG, reg_id:2}, value:128'h80009000}
//   },
//   { // pc 0x800000c
//     '{key:'{reg_type:XREG, reg_id:6}, value:128'h6000}
//   },
//   { // pc 0x8000010
//     '{key:'{reg_type:CSR, reg_id:csr_ids_pkg::CSR_MSTATUS}, value:128'h8000000a00006000}
//   },
//   { // pc 0x8000014
//     '{key:'{reg_type:FREG, reg_id:0}, value:128'hffffffff46c00000}
//   },
//   { // pc 0x8000018
//     // fsd instruction
//     192'hffff_ffff_ffff_ffff_ffff_ffff
//   }
// };

// commit_log_mem_item_t log_mem_reads_rtl[Steps][] = 
// {
//   {}, // pc 0x1000
//   {}, // pc 0x1004
//   {}, // pc 0x1008
//   { // pc 0x100c
//     '{addr: 64'h1018, wdata: 'h0, len: 8'd8}
//   }, 
//   {}, // pc 0x1010
//   {}, // pc 0x8000004
//   {}, // pc 0x8000008
//   {}, // pc 0x800000c
//   {}, // pc 0x8000010
//   {}, // pc 0x8000014
//   {}  // pc 0x8000018
// };

// commit_log_mem_item_t log_mem_write_rtl[Steps][] = 
// {
//   {}, // pc 0x1000
//   {}, // pc 0x1004
//   {}, // pc 0x1008
//   {}, // pc 0x100c
//   {}, // pc 0x1010
//   {}, // pc 0x8000004
//   {}, // pc 0x8000008
//   {}, // pc 0x800000c
//   {}, // pc 0x8000010
//   {}, // pc 0x8000014
//   {
//     '{addr: 64'h80009004, wdata: 64'hffffffff46c00000, len: 8'd8}
//   }  // pc 0x8000018
// };


// (spike) 
// core   0: 0x0000000000001000 (0x00000297) auipc   t0, 0x0
// core   0: 3 0x0000000000001000 (0x00000297) x5  0x0000000000001000
// (spike) 
// core   0: 0x0000000000001004 (0x02028593) addi    a1, t0, 32
// core   0: 3 0x0000000000001004 (0x02028593) x11 0x0000000000001020
// (spike) 
// core   0: 0x0000000000001008 (0xf1402573) csrr    a0, mhartid
// core   0: 3 0x0000000000001008 (0xf1402573) x10 0x0000000000000000
// (spike) 
// core   0: 0x000000000000100c (0x0182b283) ld      t0, 24(t0)
// core   0: 3 0x000000000000100c (0x0182b283) x5  0x0000000080000004 mem 0x0000000000001018
// (spike) 
// core   0: 0x0000000000001010 (0x00028067) jr      t0
// core   0: 3 0x0000000000001010 (0x00028067)
// (spike) 
// core   0: >>>>  start
// core   0: 0x0000000080000004 (0x00009117) auipc   sp, 0x9
// core   0: 3 0x0000000080000004 (0x00009117) x2  0x0000000080009004
// (spike) 
// core   0: 0x0000000080000008 (0xffc10113) addi    sp, sp, -4
// core   0: 3 0x0000000080000008 (0xffc10113) x2  0x0000000080009000
// (spike) 
// core   0: 0x000000008000000c (0x00006337) lui     t1, 0x6
// core   0: 3 0x000000008000000c (0x00006337) x6  0x0000000000006000
// (spike) 
// core   0: 0x0000000080000010 (0x30032073) csrs    mstatus, t1
// core   0: 3 0x0000000080000010 (0x30032073) c768_mstatus 0x8000000a00006000
// (spike) 
// core   0: 0x0000000080000014 (0xd0037053) fcvt.s.w ft0, t1
// core   0: 3 0x0000000080000014 (0xd0037053) f0  0xffffffff46c00000
// (spike) 
// core   0: 0x0000000080000018 (0x00013427) fsd     ft0, 8(sp)
// core   0: 3 0x0000000080000018 (0x00013427) mem 0x0000000080009008 0xffffffff46c00000

endmodule

