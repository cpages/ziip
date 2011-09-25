local opts = {moveUp, moveDown, moveLeft, moveRight, shoot}
local debug = false

math.randomseed(os.time())
math.random(); math.random(); math.random()

function getRandOpt()
    rval = math.random(5)
    if (debug) then
        print(rval)
    end
    return rval
end

function OnMove()
    opts[getRandOpt()](1)
end
