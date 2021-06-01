`timescale 1ns / 1ps

module counter(
    input CLOCK,
    input RESET,
    input [1:0] BUTTONS,
    output [3:0] LEDS
    );
    
    reg [3:0] count = 4'd0;
    reg clk = 0;
    reg [3:0] out = 4'd0;
    reg bs0 = 1'b0;
    reg bs1 = 1'b0;
    reg rst = 1'b0;
    
    // Turn 125Mhz clock into symetric 1Hz clock
    reg [26:0] clk_count = 28'd0;
    always @(posedge CLOCK) begin
        if (clk_count == 28'd0)
            clk <= 1;
        else if (clk_count == 28'd62499999)
            clk <= 0;
        if (clk_count == 28'd124999999) 
            clk_count <= 0;
        else
            clk_count <= clk_count + 28'd1;
    end
    
    // Always blocks won't trigger off buttons for some reason
    // I am essentially sampling the buttons state at 125MHZ
    // and creating buffer signal of the buttons that can 
    // trigger an always block
    always @(posedge CLOCK) begin
        if (bs0 != BUTTONS[1]) begin
            bs0 = BUTTONS[1];
            if (bs0 == 1)
                count = count + 4'b1;
        end
        
        if (bs1 != BUTTONS[0]) begin
            bs1 = BUTTONS[0];
            if (bs1 == 1)
                count = count - 4'b1;
        end
        
        if(rst != RESET) begin
            rst = RESET;
            if (rst == 1)
                count = 1'b0;
        end
    end
    
    
    always @(posedge clk) begin
        out[3:0] <= count[3:0];
    end
    
    assign LEDS[3:0] = out[3:0];

endmodule
