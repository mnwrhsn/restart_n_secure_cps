function Cost_total = est_cost( N_exp, P_exp, T_R, TTimer, max_t, cost, PDetect, PClean)

    delta_t = 0.01;

    Cost_total = 0;
    
    TTimer_scaled = TTimer * 100;
    max_t_scaled = max_t * 100;

    for j=1:N_exp
       %sum of all exploits
       
       cost_n = 0;
       for t = 1:TTimer_scaled
            %integral 1
            
            int1_part1 = 0 ;
            for i1 = 1:(TTimer_scaled-t)
                
                int1_part1 = int1_part1 + cost(j,i1) * PDetect(j,i1) * delta_t;
            end
            
            %integral 2
            
            int1_part2 = 0;
            for i2=(TTimer_scaled-t) + 1 :max_t_scaled
                
                int1_part2 = int1_part2 + PDetect(j,i2) * delta_t;
            end
            
            int1_part2 = int1_part2 * cost(j,(TTimer_scaled-t) + 1);
            
            int1 = int1_part1 + int1_part2;
            
            cost_n = cost_n + int1 * PClean(j,t) * delta_t ;

       end

       Cost_total = Cost_total + P_exp(j) * cost_n;

    end



end
