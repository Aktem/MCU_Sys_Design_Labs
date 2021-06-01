`timescale 1ns / 1ps

module jackpot(
    output [3:0] LEDS,
    input CLOCK,
    input [3:0] SWITCHES,
    input RESET
    );
    
    //variables needed for internal processing
    // used for clock signals
    reg clk = 0;
    reg [26:0] clk_count = 27'd0;
    // used as output buffer and state logic
    reg [3:0] current_out = 4'b0000;
    wire [3:0] next_out;
    // tracker on switched because cant directly edge trigger off of them
    reg [3:0] switch_state = 4'b0000;
    // checking if win condition
    reg win_condition = 1'b0;
    
    
    //Logic for LED cycler + if in win state
    assign next_out = (current_out==4'b0001|current_out==4'b1111) ? 4'b1000:(current_out>>1'b1);
    
    
        // Clock Divider
    always @(posedge CLOCK) begin
        if (clk_count == 27'd0)
            clk <= 1;
        else if (clk_count == 27'd31249999)
            clk <= 0;
        if(clk_count == 27'd62499999)
            clk_count <= 27'd0;
        else
            clk_count <= clk_count + 27'd1;
    end
    
    
    // reading switches sampling at CLOCK frequency 
    always @(posedge CLOCK) begin
        switch_state <= (switch_state!=SWITCHES) ? SWITCHES:switch_state;
    end
    
    
    //edge triggered win check 
    always @(switch_state) begin
        win_condition <= (switch_state == current_out) ? 1'b1:1'b0;
    end
    
    
    // assigning outputs
    always @(posedge clk) begin
        if (RESET==1'b1)
            current_out <= 4'b0001;
        else
            current_out <= (win_condition==1'b1) ? 4'b1111:next_out;
    end 
    
    assign LEDS[3:0] = current_out[3:0];
endmodule
