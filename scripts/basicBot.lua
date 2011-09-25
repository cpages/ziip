--constants and definitions
local rowLen = 7
local colLen = 5
local opts = {moveUp, moveDown, moveLeft, moveRight, shoot}
--local colors = {"Red", "Green", "Blue", "Purple", "NoColor"}
local colors = {"R", "G", "B", "P", "N"}

--local data
local slot = -1
local board = {}

--enable debug
local boardDebug = false
local playerDebug = false

math.randomseed(os.time())
math.random(); math.random(); math.random()

function getRandOpt()
    return math.random(5)
end

function updateBoard()
    local pieces = getColStat(slot)
    if (boardDebug) then
        local oStr = "rcvd:"
        for i,v in ipairs(pieces) do
            oStr = oStr .. " " .. v
        end
        print (oStr)
    end
    local it = 1
    for i = 1, 4 do
        for j = 1, rowLen do
            board[i][j] = colors[pieces[it] +1]
            it = it+1
        end
    end
    for i = 5, 8 do
        for j = 1, colLen do
            board[i][j] = colors[pieces[it] +1]
            it = it+1
        end
    end
    for i = 9, 12 do
        for j = 1, rowLen do
            board[i][j] = colors[pieces[it] +1]
            it = it+1
        end
    end
    for i = 13, 16 do
        for j = 1, colLen do
            board[i][j] = colors[pieces[it] +1]
            it = it+1
        end
    end
end

function printRow(idx, row)
    local line = "r" .. idx .. ":"
    for i,piece in ipairs(row) do
        line = line .. " " .. piece
    end
    print (line)
end

function printBoard()
    for i,row in ipairs(board) do
        printRow(i, row)
    end
end

function OnInit(nslot)
    slot = nslot
    for i = 1, 16 do
        board[i] = {}
    end
end

function OnMove()
    assert (slot ~= -1)
    updateBoard()
    if (boardDebug) then
        printBoard()
    end
    local pos = getPlayerPos(slot)
    local dir = getPlayerDir(slot)
    local color = getPlayerColor(slot)
    if (playerDebug) then
        print ("pos: " .. pos[1] .. ", " .. pos[2])
        print ("dir: " .. dir)
        print ("col: " .. color)
    end

    --for now, move and shoot randomly
    opts[getRandOpt()](slot)
end
