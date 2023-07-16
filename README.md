# RoboBrain

## My assumptions:
 - If I understand correctly, we are using rotating motors (for simplicity only clockwise). 
 - The motor positions are angular positions, and will be represented as angles 0-360 when zero is the motor starting point. 
 -  The motors main movement is by calling `netFunctions` in the `mainLoop` that control motors position. 
 - The netFunction will be added to a queue and removed in FIFO. netFunction will be exhausted in `mainLoop`, but this behaviour could be to run single `netFunction` per cycle. 
 - ALSO, the motors are constantly moving in `motorsLoop` in one direction. 
 - ALSO, the motors also have some `noise` produced in `positionLoop` that are constantly rotating but in a small portion 
 - The difference in Hz frequency will let the motors change positions with "priority where the lower Hz will get precedence like so: `NetFunction(MainLoop) -> MotorsLoop -> PositionLoop`
