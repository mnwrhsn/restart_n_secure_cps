function unavailable_time_total = unavailable_time(  N_exp, P_exp, T_R, TTimer, max_t, cost, PDetect, PClean )

    TTimer_scaled = TTimer * 100;
    max_t_scaled = max_t * 100;
    delta_t = 0.01;
    
    unavailable_time_total = 0;
    for j=1:N_exp
       %sum of all unavailable_time_total
       
       
       %compute the clean time
       T_j_clean = 0;
       for t = 1:TTimer_scaled
           T_j_clean = T_j_clean + t/100 * PClean(j,t) * delta_t ; 
       end
       
       for t = TTimer_scaled:max_t_scaled
           T_j_clean = T_j_clean + TTimer_scaled/100 * PClean(j,t) * delta_t ;
       end
       
       
       
       %compute the detect
       T_j_detect = 0;
       for t = 1:TTimer_scaled
            %integral 1
            
            int1 = 0 ;
            for i1 = 1:(TTimer_scaled-t)
                int1 = int1 + i1/100 * PDetect(j,i1) * delta_t;
            end
            
            %integral 2
            for i2=(TTimer_scaled-t) + 1 :max_t_scaled
                int1 = int1 + (TTimer_scaled-t)/100 * PDetect(j,i2) * delta_t;
            end
            
            T_j_detect = T_j_detect + int1 * PClean(j,t) * delta_t ;
       end
       
       
       %compute the period
       T_j_period = T_j_clean + T_j_detect + T_R;
       
       
       unavailable_time_total = unavailable_time_total + P_exp(j) * (1 - T_j_clean/T_j_period);

    end


end

