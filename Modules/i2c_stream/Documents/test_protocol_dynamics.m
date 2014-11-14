% This file was used to test the dynamics of the streaming protocol under poisson event arrival

events=rand(1, 10000)<0.2;

request_blocks=[];
avg=[];
underrun=0;

state=0;
blocks_read_moving_average=0;
queue=0;
queue_perceived=0;
for i=1:length(events)
	queue=queue+events(i);
	if state==0
		if queue_perceived<1
			queue_perceived=1;
		end
		blocks_read_moving_average=blocks_read_moving_average*0.96+queue_perceived*0.04;
		avg(i)=blocks_read_moving_average;
		%queue_perceived=queue_perceived+floor(blocks_read_moving_average);
		%queue_perceived=floor(blocks_read_moving_average);
		request_blocks(i)=queue_perceived;
		state=queue_perceived+5;
	elseif state<5
		state=state-1;
	elseif state==5
		queue_perceived=queue;
		state=state-1;
	else
		queue=queue-1;
		if queue<0,
			underrun=underrun+1;
			queue=0;
		end
		state=state-1;
	end
end

stem(request_blocks)
underrun

