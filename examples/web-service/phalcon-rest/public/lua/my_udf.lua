function mytransform(rec, offset)
    rec['age'] = rec['age'] + offset
    aerospike:update(rec)
end
function startswith(rec, bin_name, prefix)
      if not aerospike:exists(rec) then
              return false
                end
                  if not prefix then
                          return true
                            end
                              if not rec[bin_name] then
                                      return false
                                        end
                                          local bin_val = rec[bin_name]
                                            l = prefix:len()
                                              if l > bin_val:len() then
                                                      return false
                                                        end
                                                          ret = bin_val:sub(1, l) == prefix
                                                            return ret
                                                        end

