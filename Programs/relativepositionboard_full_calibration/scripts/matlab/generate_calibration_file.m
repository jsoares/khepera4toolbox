function [] = generate_calibration_file(folder_paths, robots, display)
% GENERATE_CALIBRATION_FILE create the calibration file such the errors
%          in range and bearing estimation are minimized.
%    GENERATE_CALIBRATION_FILE(folder_path, robots) takes the path to
%    the folder containing the the generated data files. This folder should
%    contains a 'positions.txt' file and the one or several 'rssi_robot_x.txt'
%    files. The second argument robots is vector containing the robots
%    identifier in the same order than you specified then during the
%    initial part of the calibration process.
    
    if (nargin == 2)
        display = false;
    end

    if (~iscell(folder_paths))
        folder_paths = {folder_paths};
    end
    
    training_data = extract_training_data(folder_paths{1});
    nrobots = length(robots);
    if (size(training_data,1) ~= nrobots)
        fprintf(2, 'Please specify the correct robot identifiers.\n');
        return;
    end
    for i = 2:length(folder_paths)
        td = extract_training_data(folder_paths{i});
        if (size(training_data,1) ~= nrobots)
            fprintf(2, 'Please specify a consistent set of training data.\n');
            return;
        end
        for j = 1:nrobots
            for k = 1:8
                training_data{j,k}.rssis = [training_data{j,k}.rssis; td{j,k}.rssis];
                training_data{j,k}.range = [training_data{j,k}.range; td{j,k}.range];
                training_data{j,k}.bearing = [training_data{j,k}.bearing; td{j,k}.bearing];
            end
        end
    end
    
    nrobots = length(robots);
    param = zeros(nrobots*8, 7);
    error = zeros(nrobots*8, 1);
    fprintf('Optimizing the parameters for each sector. It may take a few minutes...\n');
    for i = 1:nrobots
        for j = 1:8
            [p e] = learn_parameters(training_data{i,j}.rssis, training_data{i,j}.range, training_data{i,j}.bearing, display);
            param((i-1)*8+j, :) = p;
            error((i-1)*8+j, :) = e;
            if (display)
                fprintf('Robot %d, sector %d -> %.2f 1.00 %.2f, %.2f %.2f %.2f %.2f %.2f -> %.2f [m]\n', i, j, p(1), p(2), p(3), p(4), p(5), p(6), p(7), e);
                pause;
            end
        end
        if (display)
            fprintf('\n');
        end
    end
    
    p = mean(param);
    e = mean(error);
    if (display)
        fprintf('Average sector -> %.2f 1.00 %.2f, %.2f %.2f %.2f %.2f %.2f -> %.2f [m]\n', p(1), p(2), p(3), p(4), p(5), p(6), p(7), e);
    end
    
    % Generate file
    answer = input('Do you want to generate the calibration file? [Y/n] ', 's');
    if (strcmp(answer,'n'))
        return;
    end
    filename = input('In which file do you want to save the calibration data (../calibration_files/relativepositionboard)? ', 's');
    if (isempty(filename))
        filename = '../calibration_files/relativepositionboard';
    end
    if (exist(filename, 'file'))
        answer = input('Do you want to create a new file (c) or update that file (u)? [c/U] ', 's');
        if (isempty(answer))
            answer = 'u';
        end
        bak_filename = strcat(filename, '_', datestr(now, 'yyyymmdd_HHMM'));
        fprintf('The old %s file will be backed up as %s.\n', filename, bak_filename);
        if (strcmp(answer,'u'))
            % Read the old file first
            [p param nrobots robots] = update_file(filename, param, nrobots, robots);
        end
        movefile(filename, bak_filename);
    end
    fid = fopen(filename, 'w');
    fprintf(fid, '0 0 %.2f 1.00 %.2f %.2f %.2f %.2f %.2f %.2f\n\n', p(1), p(2), p(3), p(4), p(5), p(6), p(7));
    for i = 1:nrobots
        for j = 1:8
            p = param((i-1)*8+j, :);
            fprintf(fid, '%d %d %.2f 1.00 %.2f %.2f %.2f %.2f %.2f %.2f\n', robots(i), j-1, p(1), p(2), p(3), p(4), p(5), p(6), p(7));
        end
        fprintf(fid, '\n');
    end
    fclose(fid);
    fprintf('Calibration file \"%s\" created.\n', filename);
    
    
    function [p param nrobots robots] = update_file(filename, param, nrobots, robots)
        rb = load(filename);
        % Grab robots id
        ids = unique(rb(:,1))';
        ids = setdiff(ids, [robots 0]); % values in ids that are not in robots
        for m = 1:length(ids)
            for l = 1:8
                index = rb(1,:)==ids(k) & rb(2,:)==l;
                param((m+nrobots-1)*8+l, :) = rb(index, 3:end);
            end
        end
        % Update nrobots and robots
        robots = [robots ids];
        nrobots = length(robots);
        % Update mean value
        p = mean(param);
    end
end