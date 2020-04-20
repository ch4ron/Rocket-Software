require "#{File.dirname(__FILE__)}/../External/CMock/lib/cmock"

if $0 == __FILE__
    includes = ARGV
        Dir["STM32F4xx_HAL_Driver/Inc/*"]
    options = {}
    options[:mock_path] = "#{File.dirname(__FILE__)}/mocks"
    options[:strippables] = ['(?:.*Register.*Callback.*;)']
    options[:plugins] = [:array, :ignore, :ignore_arg, :expect_any_args]
    CMock.new(options).setup_mocks(includes)
    mocks = Dir["#{File.dirname(__FILE__)}/mocks/*"]
    for file in mocks
        content = File.read(file)
        changed = content.gsub(/#include "stm32f4xx.*.h"/, '#include "stm32f4xx_hal.h"')
        File.open(file, "w") {|file| file.puts changed}
    end
end
