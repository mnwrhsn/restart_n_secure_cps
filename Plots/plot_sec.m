clear
clc
close all

N_exp = 2


t = 1:60000;
t = t/100;

P_exp = [2/4 2/4];

%Example 1 config

cost(1,:) = 2*exp(t/130);
PClean(1,:) = normpdf(t,110,50);
PClean(1,:) = PClean(1,:)/sum(PClean(1,:))*100;
PDetect(1,:) = normpdf(t,20,30);
PDetect(1,:) = PDetect(1,:)/sum(PDetect(1,:))*100;


cost(2,:) = (sign(t) + 1)*22;
PClean(2,:) = normpdf(t,180,40);
PClean(2,:) = PClean(2,:)/sum(PClean(2,:))*100;
PDetect(2,:) = normpdf(t,30,20);
PDetect(2,:) = PDetect(2,:)/sum(PDetect(2,:))*100;


%Example 2 config

% cost(1,:) = 2*exp(t/130);
% PClean(1,:) = normpdf(t,40,50);
% PClean(1,:) = PClean(1,:)/sum(PClean(1,:))*100;
% PDetect(1,:) = normpdf(t,140,30);
% PDetect(1,:) = PDetect(1,:)/sum(PDetect(1,:))*100;
% 
% 
% cost(2,:) = (sign(t) + 1)*22;
% PClean(2,:) = normpdf(t,30,40);
% PClean(2,:) = PClean(2,:)/sum(PClean(2,:))*100;
% PDetect(2,:) = normpdf(t,110,20);
% PDetect(2,:) = PDetect(2,:)/sum(PDetect(2,:))*100;





TTimers(1:10) = (1:10) * 3; 
TTimers(1) = 1
TTimers(11:20) = (1:10) * 30; 

CYCLES = size(TTimers,2)
results = zeros(2,CYCLES);
alphas = [10 20 40 80 160 320] ; 
T_R = 10;

%Compute the total cost of compromise
parfor i = 1:CYCLES;
    i
    
    TTimer = TTimers(i);
    Cost_total = est_cost( N_exp, P_exp, T_R, TTimer, max(t), cost, PDetect, PClean)
%     
    results(1,i) = Cost_total;
%     
end

display('Unavailable Time started')

%Compute the available time of system
parfor i = 1:CYCLES
    i
    
    TTimer = TTimers(i)
    
    unavailable_time_total = unavailable_time(  N_exp, P_exp, T_R, TTimer, max(t), cost, PDetect, PClean )
    
    results(2,i) = unavailable_time_total;
end


total_min_function = zeros(size(alphas,2),10) ;
for j = 1:size(alphas,2)
    for i = 1:CYCLES
        total_min_function(j,i) = results(1,i) + alphas(j) * results(2,i);
    end
end




%%
close all

h = figure('position',[100 200 300 150])

hold on
plot(t,cost(1,:),'black','LineWidth',2)
plot(t,cost(2,:),'red','LineWidth',2)
axis([0 max(t) -1 100])
legend({'Attack A','Attack B'},'FontSize',13,'FontWeight','bold')
xlabel('Time (s)','FontSize',13,'FontWeight','bold')
ylabel('Damage Function','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'damage_fun','-dpdf','-r0')

h = figure('position',[100 500 300 150])

hold on
plot(t,PClean(1,:),'black','LineWidth',2)
plot(t,PClean(2,:),'red','LineWidth',2)
% axis([0 max(t) -1 1])
legend({'Attack A','Attack B'},'FontSize',13,'FontWeight','bold')
xlabel('Time (s)','FontSize',13,'FontWeight','bold')
ylabel('P_{Com} PDF','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'p_clean','-dpdf','-r0')


h = figure('position',[100 700 300 150])

hold on
plot(t,PDetect(1,:),'black','LineWidth',2)
plot(t,PDetect(2,:),'red','LineWidth',2)
% axis([0 max(t) -1 1])
legend({'Attack A','Attack B'},'FontSize',13,'FontWeight','bold')
xlabel('Time (s)','FontSize',13,'FontWeight','bold')
ylabel('P_{Detect} PDF','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'p_detect','-dpdf','-r0')

h = figure('position',[100 1000 300 150])

hold on

plot(TTimers,results(1,:),'blue','LineWidth',2)
% plot(t,cost(2,:),'red','LineWidth',2)
% axis([0 max(t) -1 50])
% legend({'Cost 1','Cost 2'},'FontSize',13,'FontWeight','bold')
xlabel('T_{Timer} (s)','FontSize',13,'FontWeight','bold')
ylabel('Exp(\Sigma Damage^j)','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'exp_cost','-dpdf','-r0')



h = figure('position',[100 1300 300 160])

hold on

plot([0, TTimers],[1, results(2,:)], 'blue','LineWidth',2)
% plot(t,cost(2,:),'red','LineWidth',2)
% axis([0 max(t) -1 50])
% legend({'Cost 1','Cost 2'},'FontSize',13,'FontWeight','bold')
xlabel('T_{Timer} (s)','FontSize',13,'FontWeight','bold')
ylabel('Exp(Unavailablity) (%)','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'exp_unavailable','-dpdf','-r0')


h = figure('position',[100 1600 300 150])

hold on

plot(TTimers,total_min_function(1,:), 'yellow','LineWidth',2)
plot(TTimers,total_min_function(2,:), 'red','LineWidth',2)
plot(TTimers,total_min_function(3,:), 'blue','LineWidth',2)
plot(TTimers,total_min_function(4,:), 'green','LineWidth',2)
plot(TTimers,total_min_function(5,:), 'cyan','LineWidth',2)
plot(TTimers,total_min_function(6,:), 'black','LineWidth',2)

% plot(t,cost(2,:),'red','LineWidth',2)
% axis([0 max(t) -1 50])

columnlegend(2,{'\alpha 10','\alpha 20','\alpha 40','\alpha 80','\alpha 160','\alpha 320'},'FontSize',13,'FontWeight','bold')
xlabel('T_{Timer} (s)','FontSize',13,'FontWeight','bold')
ylabel('Cost Function','FontSize',13,'FontWeight','bold')
hold off


set(h,'Units','Inches');
pos = get(h,'Position');
set(h,'PaperPositionMode','Auto','PaperUnits','Inches','PaperSize',[pos(3), pos(4)])
print(h,'cost_fun','-dpdf','-r0')
