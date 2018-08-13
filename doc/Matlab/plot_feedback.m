function plot_feedback(data, limits)

dim = size(data);
samples = dim(1);
streams = dim(2);

% graph with absolute feedback values
figure
subplot(2,1,1)
title 'Absolute filtered feedback'
hold on
for i = (1:2:streams)
    plot(data(:,i))
end
ylim([0 1024])
hold off

subplot(2,1,2)
title 'Absolute raw feedback'
hold on
for i = (2:2:streams)
    plot(data(:,i))
end
ylim([0 1024])
hold off

% graph with relative feedback values
scaled_data = zeros(samples, streams);
for i = (1:streams)
    k = ceil(i / 2);
    scaled_data(:,i) = scale_stream(data(:,i), limits(1,k), limits(2,k));
end

figure
subplot(2,1,1)
title 'Scaled filtered feedback'
hold on
for i = (1:2:streams)
    plot(scaled_data(:,i))
end
ylim([0 1])
hold off

subplot(2,1,2)
title 'Scaled raw feedback'
hold on
for i = (2:2:streams)
    plot(scaled_data(:,i))
end
ylim([0 1])
hold off

end