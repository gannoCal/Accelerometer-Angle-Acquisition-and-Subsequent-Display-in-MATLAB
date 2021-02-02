serialportlist()
s=serialport("COM3",38400)
angle_arr=zeros(60,1);
i=0
hf=figure
h = plot(angle_arr);
axis([0 60 -180 180])
yticks([-180:20:180])
title('Angle Data of x-axis Plotted in Real-time');
xlabel('Time(s)')
ylabel('Angle (degrees)')
xticks([0 10 20 30 40 50 60])
xticklabels({'0','1','2','3','4','5','6'})
p=0;
while true
    val=readline(s)
    p=p+1;
    if mod(p,10)==0
        p=0;
        
        if i<60
        i=i+1;

        angle_arr(i)=val;
        else
           for j=1:59
              angle_arr(j)=angle_arr(j+1); 
           end
           angle_arr(60)=val;
        end
        set(h,'YData', angle_arr);
        refreshdata(hf,'caller')
        drawnow
    end
    
    
end