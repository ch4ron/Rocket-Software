require '../External/CMock/lib/cmock'

if $0 == __FILE__
    includes = Dir["Source/include/*"]
    options = {}
    options[:strippables] = [:PRIVILEGED_FUNCTION, :portDONT_DISCARD ]
    options[:plugins] = [:array, :ignore, :ignore_arg, :expect_any_args]
    CMock.new(options).setup_mocks(includes)
end
