function mytransform(rec)
    rec['age'] = rec['age'] + offset
    aerospike:update(rec)
end
