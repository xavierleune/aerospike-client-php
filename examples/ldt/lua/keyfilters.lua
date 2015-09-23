local keyfilters = {}

function keyfilters.even_filter(element)
    local k = element['key'] % 2
    if k == 0 then
        return element
    else
        return nil
    end
end

function keyfilters.range_filter(element, from, to)
    if element['key'] >= from and element['key'] <= to then
        return element
    else
        return nil
    end
end

return keyfilters
