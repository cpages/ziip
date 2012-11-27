--constants and definitions
local rowLen = 7
local colLen = 5
local dirs = {"U", "D", "L", "R", "N"}
local actions = {U=moveUp, D=moveDown, L=moveLeft, R=moveRight, S=shoot, N=noAction}
local colors = {"R", "G", "B", "P", "N"}
local rowAid = {}

--local data
local slot = -1
local board = {}

--enable debug
local boardDebug = false
local playerDebug = false
local algDebug = false
local deepDebug = false

math.randomseed(os.time())
math.random(); math.random(); math.random()

--general aux functions
function getRandOpt()
    return math.random(4)
end

function printTable(table)
    local str = "table:"
    for i,v in ipairs(table) do
        str = str .. " " .. v
    end
    print (str)
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

function initRowAid()
    for i = 1, 4 do
        rowAid[i] = {orient="H", y=i-1, side="L"}
        rowAid[i+4] = {orient="V", x=i-1, side="D"}
        rowAid[i+8] = {orient="H", y=4-i, side="R"}
        rowAid[i+12] = {orient="V", x=4-i, side="U"}
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

function getHeadColor(row)
    local numPieces = 0
    local headColor = "N"
    for i,piece in ipairs(row) do
        if (piece ~= "N") then
            numPieces = numPieces + 1
            headColor = piece
        end
    end
    return headColor, numPieces
end

--find closer piece of the same color
function findColor(color)
    local possibleTargets = {}
    for i,row in ipairs(board) do
        local headColor, numPieces = getHeadColor(row)
        if (algDebug and deepDebug) then
            print (i .. ":" .. headColor .. ", " .. numPieces)
        end
        if (headColor == color) then
            table.insert (possibleTargets, i)
        end
    end
    if (algDebug) then
        printTable(possibleTargets)
    end
    return possibleTargets
end

function findCostAndNextMove(pos, dir, row)
    local nextMove = "N"
    local cost = 0
    if (row.orient == "H") then
        cost = math.abs(pos.y - row.y)
        if (cost ~= 0) then
            if (row.y < pos.y) then
                nextMove = "U"
            else
                nextMove = "D"
            end
        end
        if (dir ~= row.side) then
            cost = cost+1
            if (nextMove == "N") then
                nextMove = row.side
            end
        end
        if (cost == 0) then
            nextMove = "S"
        end
    else
        cost = math.abs(pos.x - row.x)
        if (cost ~= 0) then
            if (row.x < pos.x) then
                nextMove = "L"
            else
                nextMove = "R"
            end
        end
        if (dir ~= row.side) then
            cost = cost+1
            if (nextMove == "N") then
                nextMove = row.side
            end
        end
        if (cost == 0) then
            nextMove = "S"
        end
    end
    return cost, nextMove
end

--find repeated colors
function findRepeatedColor()
    local headCount = {}
    for i,row in ipairs(board) do
        local headColor, numPieces = getHeadColor(row)
        if (algDebug and deepDebug) then
            print (i .. ":" .. headColor .. ", " .. numPieces)
        end
        if (headColor == color) then
            table.insert (possibleTargets, i)
        end
    end
    if (algDebug) then
        printTable(possibleTargets)
    end
    return possibleTargets
end

function OnInit(nslot)
    slot = nslot
    for i = 1, 16 do
        board[i] = {}
    end
    initRowAid()
end

function OnMove()
    assert (slot ~= -1)
    updateBoard()
    if (boardDebug) then
        printBoard()
    end
    local cPos = getPlayerPos(slot)
    local cDir = getPlayerDir(slot)
    local cColor = getPlayerColor(slot)
    local pPos = {x=cPos[1], y=cPos[2]}
    local pDir = dirs[cDir+1]
    local pColor = colors[cColor+1]
    if (playerDebug) then
        print ("pos: " .. pPos.x .. ", " .. pPos.y)
        print ("dir: " .. pDir)
        print ("col: " .. pColor)
    end

    --find and shoot pieces of the current player color
    local possibleTargets = findColor(pColor)
    local cost = 10
    local nextMove = "N"
    for i,v in ipairs(possibleTargets) do
        local tCost, tMove = findCostAndNextMove(pPos, pDir, rowAid[v])
        if (algDebug) then
            print (v .. ":" .. tCost .. ", " .. tMove)
        end
        if (tCost < cost) then
            cost = tCost
            nextMove = tMove
        end
    end

    if (nextMove == "N") then
        local headCounts = findRepeatedColor()
    end

    --TODO: deal with critical situations

    if (nextMove == "N") then
        --random movement (no shots)
        actions[dirs[getRandOpt()]](slot)
    else
        actions[nextMove](slot)
    end
end
