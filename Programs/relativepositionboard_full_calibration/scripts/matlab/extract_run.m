function [nrobots, t, X, Y, A, rssis] = extract_run(folder_path, filter)
% EXTRACT_RUN Extracts all data of the specified calibration run.
%             The data is smoothed and put into proper variables.
%    EXTRACT_RUN(folder_path) takes the path to the folder containing the 
%    the generated data files. This folder should contains a
%    'positions.txt' file and the one or several 'rssi_robot_x.txt' files.

    if (nargin == 1)
        filter = false;
    end

    % Try to open the precomputed data file if it exists
    precomputed_filename = sprintf('%s/precomputed.mat', folder_path);
    if (exist(precomputed_filename, 'file'))
        load(precomputed_filename);
    else
        % Open the swistrack file
        filename = sprintf('%s/positions.txt', folder_path);
        positions = load(filename);
        nrobots = (size(positions, 2)-1)/3;
        if (nrobots < 2)
            fprintf(2, 'There are not enough robots (at least 2 are needed).\n');
            return;
        end
        fprintf('%d robots were detected in this calibration run.\n', nrobots);
        % Rename position data
        t = positions(:,1) - positions(1,1);
        X = positions(:,2:3:end);
        Y = positions(:,3:3:end);
        A = positions(:,4:3:end);
        % Smooth data
        fprintf('We are smoothing the trajectories of Swistrack. It may take a few minutes...\n');
        for i = 1:nrobots
            X(:,i) = smooth(X(:,i), 10/length(t),'rloess');
            Y(:,i) = smooth(Y(:,i), 10/length(t),'rloess');
            cosA = smooth(cos(A(:,i)), 10/length(t),'rloess');
            sinA = smooth(sin(A(:,i)), 10/length(t),'rloess');
            A(:,i) = unwrap(atan2(sinA, cosA));
        end
        
        % Load RSSI data
        rssis = cell(nrobots, nrobots);
        for i = 1:nrobots
            rssi_filename = sprintf('%s/rssi_robot_%d.txt', folder_path, i-1);
            r = load(rssi_filename);
            for j = 1:nrobots
                r_individual = r(r(:,3) == j-1, :);
                if (filter)
                    % Filter out some weird values (zeros in the rssi)
                    index = find(sum(r_individual(:,4:end) == 0, 2) == 0);
                    if (size(r_individual, 1) ~= 0)
                        fprintf('%.2f %% of the RSSIs were filtered out (%d <- %d). I2C?\n', 100-100*length(index)/size(r_individual, 1), i, j);
                    end
                else
                    index = 1:size(r_individual, 1);
                end
                rssis{i,j}.t = r_individual(index,1) - positions(1,1);
                rssis{i,j}.values = [(1:length(rssis{i,j}.t))' r_individual(index,4:end)];
            end
        end
        
        % Save data
        save(precomputed_filename, 'nrobots', 't', 'X', 'Y', 'A', 'rssis');
    end
    
end