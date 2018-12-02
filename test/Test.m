% run LoadVI in ..\MATLAB directory to register path
function Test(ctrl)
    %global res
    %res = ctrl;
    res = CtrlGetValue(ctrl);
    Console([num2str(ctrl),' ',num2str(res),13]);
end