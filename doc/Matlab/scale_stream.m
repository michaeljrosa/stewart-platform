function [scaled] = scale_stream(data, max, min)

scaled = (data - min) / (max - min);

end