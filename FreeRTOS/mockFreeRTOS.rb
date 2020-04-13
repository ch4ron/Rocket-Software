require '../External/CMock/lib/cmock'

if $0 == __FILE__
    includes = Dir["Source/include/*"]
    print includes
    options = {}
    options[:strippables] = [:PRIVILEGED_FUNCTION, :portDONT_DISCARD ]
    CMock.new(options).setup_mocks(includes)
end
