module cosim_ornek_kullanim;
  import cosim_pkg::*;

  commit_log_reg_item_t log_reg_write_from_c [CommitLogEntries];
  commit_log_mem_item_t log_mem_read_from_c [CommitLogEntries];
  commit_log_mem_item_t log_mem_write_from_c [CommitLogEntries];
  int num_elements_inserted_from_c_side; // 3'u icin de kullaniliyor.
  reg_t temp_key;
  freg_t temp_value;
  reg_t temp_pc;
  initial begin: cosimulation
    init();

    for (;;) begin: simulation_loop
      if (simulation_completed()) begin // htif_t::exitcode != 0
        $display("simulation completed");
        break;
      end


      get_pc(temp_pc);
      $display("pc before execution: %0h", temp_pc);

      step();

      get_log_reg_write(log_reg_write_from_c,  num_elements_inserted_from_c_side);

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

endmodule

