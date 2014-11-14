function [training_data] = extract_training_data(folder_path)

    % Try to open the precomputed data file if it exists
    precomputed_filename = sprintf('%s/training.mat', folder_path);
    if (exist(precomputed_filename, 'file'))
        load(precomputed_filename);
    else
        % Load data
        [nrobots, t, X, Y, A, rssis] = extract_run(folder_path, true);

        % Foreach robot and each rssis
        training_data = cell(nrobots, 8);
        for i = 1:nrobots
            % Initialize arrays
            for j = 1:8
                training_data{i,j}.rssis = [];
                training_data{i,j}.range = [];
                training_data{i,j}.bearing = [];
            end
            % Populate data
            for j = 1:nrobots
                for k = 1:size(rssis{i,j}.values,1)
                    time = rssis{i,j}.t(k);
                    rssi = rssis{i,j}.values(k,2:end);

                    % Get the highest RSSI sector (3 RSSIs)
                    [rl, rc, rr, index] = get_highest_sector(rssi);
                    if (abs(rl) > 17000 || abs(rc) > 17000 || abs(rr) > 17000)
                        continue;
                    end

                    % Angle of the highest RSSI sector
                    sector_angle =  -pi/4*(index-1);

                    % Get positions of the robots
                    x = interp1(t, X, time);
                    y = interp1(t, Y, time);
                    a = interp1(t, A, time);
                    if (isnan(x))
                        continue;
                    end

                    % Compute range and bearing
                    dx = x(j)-x(i);
                    dy = y(j)-y(i);
                    range = sqrt(dx^2+dy^2);
                    bearing = format_angle(atan2(dy,dx)-a(i)-sector_angle);
                    
                    if (abs(bearing) > pi/8)
                        continue;
                    end

                    % Fill the arrays
                    training_data{i,index}.rssis = [training_data{i,index}.rssis; rl rc rr];
                    training_data{i,index}.range = [training_data{i,index}.range; range];
                    training_data{i,index}.bearing = [training_data{i,index}.bearing; bearing];
                end
            end
        end
        % Save data
        save(precomputed_filename, 'training_data');
    end
    
    function [rl_, rc_, rr_, index_] = get_highest_sector(rssi_)
        rssil_ = rssi_(mod((1:end)-2, end)+1);
        rssir_ = rssi_(mod((1:end), end)+1);
        [~, index_] = max(rssil_ + 2*rssi_ + rssir_);
        index_ = index_(1);
        rl_ = rssil_(index_);
        rc_ = rssi_(index_);
        rr_ = rssir_(index_);
    end

    function [a_] = format_angle(a_)
        while (a_ > pi),  a_ = a_-2*pi; end
        while (a_ < -pi), a_ = a_+2*pi; end
    end
end