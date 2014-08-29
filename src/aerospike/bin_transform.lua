function bin_udf_operation_integer(record, bin_name, x, y)
    record[bin_name] = (record[bin_name] * x) + y
    aerospike:update(record)
    return record[bin_name]
end

function bin_udf_operation_string(record, bin_name, str)
    if (type(record[bin_name]) == "string" or type(record[bin_name]) == "number") and
       (type(str) == "string" or type(str) == "number") then
       return record[bin_name] .. str
    end
end

function bin_udf_operation_bool(record , bin_name)
    aerospike:remove(record)
    return record[bin_name]
end

function access_list_udf(rec, bin22)
    local put_list = list()
    list.append(put_list, rec[bin22])
    list.prepend(put_list, 894)
    rec[bin22] = put_list;
    aerospike:update(rec)
end

function access_map_udf(rec, bin22)
    local put_map = map{rec[bin22] };
    rec[bin22] = put_map;
    aerospike:update(rec)
end


