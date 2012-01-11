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
# y = data(:, 14:16);
data = load('../calibData5.csv');
X = data(:, 1:13);
y = data(:, 14:15);
m = length(y);

% Calculate the parameters from the normal equation
theta = normalEqn(X, y);

% Display normal equation's result
fprintf('Theta computed from the normal equations: \n');
% fprintf(' %f \n', theta);
fprintf('\n');


price = 0; % You should change this
% price = [1, 1650, 3.5] * theta; 

sqrt(sum(((X * theta - y).^2)' )');
%%sum((X * theta - y).^2)/m

# testtry3D = [1, 384.8235, 157.2353, 424.8571, 208.4286, -24.8278, 0.000256241, 0.000171495, -4.82881, -4.78823, -0.000899328, -1.62637, -1.53846, 0.923077];
# solution = testtry3D * theta
# fprintf("should be 419.082969427109,-1.60571467131376,77.0522654056549");

testtry = [167.6399, 59.86364,189.7391,105.5901,-20.000195,0.000256,0.000171,-10.000172,-0.00181,-0.000899,0.043956,0.131868,35.032967];
solution = testtry * theta
fprintf("should be: 4,6")

squarederror = zeros(m, 2);
for i= 1:m
	solution = X(i,:) * theta;
	squarederror(i,:) = (solution - y(i,:)).^2;
	% fprintf("should be: %d, %d\n", y(i,1), y(i,2));
end
squarederror;
meansquerror = sum(squarederror)/m