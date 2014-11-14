function [param dist_error] = learn_parameters(rssis_o, ranges_o, bearings_o, display)

    if (isempty(ranges_o)) 
        param = [1 1 0 2000 1.5];
        dist_error = NaN;
        return;
    end
    
    if (nargin == 3)
        display = false;
    end

    % Options for fmincon
    options = optimset('LargeScale','off', 'Algorithm', 'active-set', ...
                       'Display','iter', 'GradObj', 'off', 'GradConstr', 'off', ...
                       'DerivativeCheck', 'off', 'HessUpdate', 'bfgs', ...
                       'NonlEqnAlgorithm', 'gn', ...
                       'LineSearchType', 'cubicpoly', ...
                       'MaxFunEvals', 2000, ...
                       'DiffMinChange', 1e-6, ...
                       'TolFun',1e-9,'TolCon',1e-9, 'TolX',1e-9);
                   
    % Initial guess (from average readings of multiple receivers)
    % scaling_left, scaling_right, alpha, beta, gamma, u, v (range = alpha*exp(-(rssi/beta)^gamma) + u + rssi/v)
    param0      = [   1   1   3.0 2000.0 1.0  0.5 12750];
    param_lower = [ 0.5 0.5   1.5  500.0 0.8 -1.0 5000];
    param_upper = [ 5.0 5.0  10.0 4000.0 3.0  1.0 20000];
    param = param0;
    param_best = param;
    error_best = 1000.0;

    % Optimize 3 times
    for i = 1:3
        % Take 60% of data points
        index = randsample(length(ranges_o), ceil(length(ranges_o)*0.6));
        rssis = rssis_o(index, :);
        ranges = ranges_o(index);
        bearings = bearings_o(index);

        % Precomputed this
        true_x = ranges.*cos(bearings);
        true_y = ranges.*sin(bearings);

        % Minimize bearing error
        % param1 = fmincon(@(x) error_bearing(x), param(1:2), [], [], [], [], param_lower(1:2), param_upper(1:2), nonlcon, options);
        % param2 = fmincon(@(x) error_range(x, param(1:2)), param(3:end), [], [], [], [], param_lower(3:end), param_upper(3:end), nonlcon, options);
        % param = [param1 param2];

        param = fmincon(@(x) error(x), param, [], [], [], [], param_lower, param_upper, nonlcon, options);
        
        % Compute real error
        dist_error = sqrt(error_dist(param)/(length(ranges)-1));
        if (dist_error < error_best)
            param_best = param;
            error_best = dist_error;
        end
    end
        
    param = param_best;
    dist_error = error_best;
    
    if (display)
        rssis = rssis_o;
        ranges = ranges_o;
        bearings = bearings_o;

        figure(1); clf;

        subplot 131; hold on;
        plot(true_x, true_y,'b.');
        [estimated_x estimated_y] = get_xy(param);
        plot(estimated_x, estimated_y,'rx');
        axis([0 3 -1.5 1.5]);
        xlabel('x [m]'); ylabel('y [m]');
        title('Position estimated');
        legend('True positions', 'Estimated positions');

        subplot 132; hold on;
        scaling_left  = param(1);
        scaling_right = param(2);
        b = (scaling_left*rssis(:,1) + 2*rssis(:,2) + scaling_right*rssis(:,3))./(2*cos(pi/4) + 2);
        c = (scaling_left*rssis(:,1) - scaling_right*rssis(:,3))./(2*sin(pi/4));
        estimated_bearing = atan2(c, b);
        plot(estimated_bearing, bearings, 'rx');
        plot([-pi/4 pi/4], [-pi/4 pi/4], 'k-');
        title('Bearing');
        xlabel('Estimated bearing [rad]'); ylabel('True bearing [rad]');
        axis equal;
        legend('Estimated relation', 'True relation');

        subplot 133; hold on;
        alpha = param(3);
        beta  = param(4);
        gamma = param(5);
        u     = param(6);
        v     = param(7);
        corrected_rssi = sqrt(b.^2+c.^2);
        plot(ranges, corrected_rssi, 'b.');
        estimated_range = alpha*exp(-(corrected_rssi/beta).^gamma) + u - corrected_rssi/v;
        plot(estimated_range, corrected_rssi, 'rx');
        xlabel('Range [m]'); ylabel('RSSI [raw]');
        title('Range');
        legend('True range', 'Estimated range');
    end

    function e = error(x)
        [estimated_x estimated_y] = get_xy(x);
        e = sum(((estimated_x - true_x).^2 + (estimated_y - true_y).^2));
    end

    function e = error_dist(x)
        [estimated_x estimated_y] = get_xy(x);
        e = sum(((estimated_x - true_x).^2 + (estimated_y - true_y).^2));
    end

    function e = error_bearing(x)
        sl = x(1);
        sr = x(2);
        b = (sl*rssis(:,1) + 2*rssis(:,2) + sr*rssis(:,3))./(2*cos(pi/4) + 2);
        c = (sl*rssis(:,1) - sr*rssis(:,3))./(2*sin(pi/4));
        estimated_bearing = atan2(c, b);
        e = sum((estimated_bearing - bearings).^2);
    end

    function e = error_range(x, x1)
        sl = x1(1);
        sr = x1(2);
        alpha = x(1);
        beta = x(2);
        gamma = x(3);
        u = x(4);
        v = x(5);
        b = (sl*rssis(:,1) + 2*rssis(:,2) + sr*rssis(:,3))./(2*cos(pi/4) + 2);
        c = (sl*rssis(:,1) - sr*rssis(:,3))./(2*sin(pi/4));
        corrected_rssi = sqrt(b.^2+c.^2);
        estimated_range = alpha*exp(-(corrected_rssi/beta).^gamma) - corrected_rssi/v + u;
        e = sum((estimated_range - ranges).^2);
    end

    function [estimated_x estimated_y] = get_xy(x)
        sl = x(1);
        sr = x(2);
        alpha = x(3);
        beta = x(4);
        gamma = x(5);
        u = x(6);
        v = x(7);
        
        b = (sl*rssis(:,1) + 2*rssis(:,2) + sr*rssis(:,3))./(2*cos(pi/4) + 2);
        c = (sl*rssis(:,1) - sr*rssis(:,3))./(2*sin(pi/4));
        
        estimated_bearing = atan2(c, b);
        corrected_rssi = sqrt(b.^2+c.^2);
        estimated_range = alpha*exp(-(corrected_rssi/beta).^gamma) - corrected_rssi/v + u;
        
        estimated_x = estimated_range.*cos(estimated_bearing);
        estimated_y = estimated_range.*sin(estimated_bearing);
    end

    function [g_ h_] = nonlcon(~)
        g_ = [];
        h_ = [];
    end
end