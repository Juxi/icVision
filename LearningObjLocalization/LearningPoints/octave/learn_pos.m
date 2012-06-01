%% Machine Learning Online Class
%  Exercise 1: Linear regression with multiple variables
%

%% Initialization

%% ================ Part 1: Feature Normalization ================

%% Clear and Close Figures
clear all; close all; clc

%%%     fprintf('Loading data ...\n');
%%%     
%%%     %% Load Data
%%%     data = load('../reddot_calibData3d_dec6.csv');
%%%     X = data(:, 1:13);
%%%     y = data(:, 14:16);
%%%     m = length(y);
%%%     
%%%     # X = X(1:60,:);
%%%     # y = y(1:60,:);
%%%     # m = length(y);
%%%     
%%%     % % Print out some data points
%%%     % fprintf('First 10 examples from the dataset: \n');
%%%     % X(1:10,:)
%%%     % y(1:10,:)
%%%     % 
%%%     % fprintf('Program paused. Press enter to continue.\n');
%%%     % pause;
%%%     
%%%     % Scale features and set them to zero mean
%%%     fprintf('Normalizing Features ...\n');
%%%     
%%%     [X mu sigma] = featureNormalize(X);
%%%     
%%%     % Add intercept term to X
%%%     X = [ones(m, 1) X];
%%%     
%%%     %% ================ Part 2: Gradient Descent ================
%%%     fprintf('Running gradient descent ...\n');
%%%     
%%%     % Choose some alpha value
%%%     alpha = 0.1; 
%%%     num_iters = 500;
%%%     
%%%     % Init Theta and Run Gradient Descent 
%%%     theta = zeros(14, size(y,2));
%%%     [theta, J_history] = gradientDescentMulti(X, y, theta, alpha, num_iters);
%%%     
%%%     % Plot the convergence graph
%%%     figure;
%%%     plot(1:numel(J_history), J_history, '-b', 'LineWidth', 2);
%%%     xlabel('Number of iterations');
%%%     ylabel('Cost J');
%%%     
%%%     % Display gradient descent's result
%%%     fprintf('Theta computed from gradient descent: \n');
%%%     fprintf(' %f \n', theta);
%%%     fprintf('\n');
%%%     
%%%     
%%%     testtry = [384.8235, 157.2353, 424.8571, 208.4286, -24.8278, 0.000256241, 0.000171495, -4.82881, -4.78823, -0.000899328, -1.62637, -1.53846, 0.923077];
%%%     
%%%     solution = [1, (testtry-mu)./sigma] * theta
%%%     fprintf("should be 419.082969427109,-1.60571467131376,77.0522654056549");
%%%     
%%%     %% ================ Part 3: Normal Equations ================

fprintf('Solving with normal equations...\n');

%% Load Data
# data = load('../reddot_calibData3d_dec6.csv');
# X = data(:, 1:13);
# y = data(:, -2:-1);
data = load('../WedThurAll.csv');
size(data)
X = data(:, 1:13);
y = data(:, 23:24) * 6 / 100;
m = length(y);

% Calculate the parameters from the normal equation
theta = normalEqn(X, y)

% Display normal equation's result
# fprintf('Theta computed from the normal equations: \n');
# fprintf(' %f \n', theta);
# fprintf('\n');


price = 0; % You should change this
% price = [1, 1650, 3.5] * theta; 
sqrt(sum(((X * theta - y).^2)' )');
%%sum((X * theta - y).^2)/m

# testtry3D = [1, 384.8235, 157.2353, 424.8571, 208.4286, -24.8278, 0.000256241, 0.000171495, -4.82881, -4.78823, -0.000899328, -1.62637, -1.53846, 0.923077];
# solution = testtry3D * theta
# fprintf("should be 419.082969427109,-1.60571467131376,77.0522654056549");

testtry = [237.55,363.35,	260.75,	383.9375,	-20.0007,	0.000256241,	0.000171495,	-0.000446343,	10.004,	-0.000899328,	-15.0769,	-9.97802,	20]; %,	4,	6];
solution = testtry * theta;
##fprintf("should be: 4,6");
fprintf('something...')
squarederror = zeros(m, 1);
for i= 1:m
	sol(i,:) = X(i,:) * theta;
	solution(i,:) = abs(X(i,:) * theta - y(i,:));
	squarederror(i,:) = solution(i,1).^2 + solution(i,2).^2;
	% fprintf("should be: %d, %d\n", y(i,1), y(i,2));
end
mi = min(solution)
ma = max(solution)
av = mean(solution)
med = median(solution)
# squarederror;
meansquerror = sqrt(	median(squarederror))
mimeansquerr = sqrt(	min(squarederror)	)
maxsquerror  = sqrt(	max(squarederror)	)