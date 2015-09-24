local keyfilters = {}

function keyfilters.even_filter(element)
    local remainder = -1
    if type(element) == "number" then
        remainder = element % 2
    elseif (getmetatable(element) == getmetatable(map())) and element['key'] then
        remainder = element['key'] % 2
    end
    if remainder == 0 then
        return element
    else
        return nil
    end
end

function keyfilters.range_filter(element, range)
    local key = nil
    if type(element) == "number" or type(element) == "string" then
        key = element
    elseif (getmetatable(element) == getmetatable(map())) and element['key'] then
        key = element['key']
    end
    if key >= range[1] and key <= range[2] then
        return element
    else
        return nil
    end
end

return keyfilters
