#!/usr/bin/env python
import click
import os
import serial
import sys

boards = ['Kromek', 'Staszek', 'Czapla', 'Pauek']
targets = ['Debug', 'Simulate', 'Test']

# Calculates the return value
def execute(cmd):
    res = os.system(cmd) >> 8
    if res != 0:
        sys.exit(res)

class Board:
    def __init__(self, name, port=None):
        board = name.capitalize()
        if board not in boards:
            click.echo('Board {} not supported, choose from: {}'.format(board, boards))
            sys.exit(-1)
        if port:
            self._port = port
        else:
            self._port = '/dev/ttyACM0'
        self._name = name.capitalize()

    def build_target(self, target, verbose=False):
        directory = self._get_build_directory(target)
        if not os.path.exists(directory):
            os.mkdir(directory)
        if verbose:
            cmd = 'cd {}; cmake ../  -DCMAKE_BUILD_TYPE={} -DVERBOSE_TEST_OUTPUT=1; make -j4'.format(directory, target.capitalize())
        else:
            cmd = 'cd {}; cmake ../  -DCMAKE_BUILD_TYPE={} -DVERBOSE_TEST_OUTPUT=0; make -j4'.format(directory, target.capitalize())
        execute(cmd)

    def clean(self):
        for target in targets:
            self._clean_target(target)

    def purge(self):
        for target in targets:
            self._purge_target(target)

    def flash(self):
        self.build_target('Debug')
        cmd = self._flash_cmd('Debug')
        execute(cmd)

    def test(self, verbose=False):
        self.build_target('Test', verbose)
        cmd = self._flash_cmd('Test')
        res = os.popen(cmd).read()
        if res:
            sys.exit(res)
        ser = serial.Serial(self._port, 115200)
        print "\r\nTESTS START\r\n"
        while True:
            line = ser.readline()
            if 'Elon!' in line:
                return
            sys.stdout.write(line)

    def _get_build_directory(self, target):
        return '{}/cmake-build-{}'.format(self._name, target.lower())

    def _clean_target(self, target):
        directory = self._get_build_directory(target)
        if not os.path.exists(directory):
            return
        print 'Cleaning', directory
        cmd = 'cd {}; make clean'.format(directory, target)
        execute(cmd)

    def _purge_target(self, target):
        directory = self._get_build_directory(target)
        cmd = 'rm -rf {}'.format(directory)
        print 'Purging', directory
        execute(cmd)

    def _flash_cmd(self, target):
        return "openocd -c 'tcl_port disabled' -s openocd/scripts -c 'gdb_port 3333' -c 'telnet_port 4444' -f " \
               "st_nucleo_f4.cfg -c 'program {}/cmake-build-{}/{}.elf' -c reset -c shutdown".format(self._name, target.lower(), self._name.capitalize())


@click.group()
@click.argument('board')
@click.pass_context
def main(ctx, board):
    ctx.obj['board'] = board
    pass


@main.command(help='Build target for board')
@click.option('-s', '--simulate', is_flag=True, help='Build simulation target')
@click.option('-t', '--test', is_flag=True, help='Build test target')
@click.pass_context
def build(ctx, simulate, test):
    board = ctx.obj['board']
    b = Board(board)
    if not simulate:
        b.build_target('Debug')
    else:
        b.build_target('Simulate')


@main.command(help='Clean all targets for board')
@click.pass_context
def clean(ctx):
    board = ctx.obj['board']
    b = Board(board)
    b.clean()


@main.command(help='Purge all targets for board')
@click.pass_context
def purge(ctx):
    board = ctx.obj['board']
    b = Board(board)
    b.purge()


@main.command(help='Run test on simulator for board')
@click.pass_context
@click.option('-v', '--verbose', is_flag=True, help='Show verbose test output')
def simulate(ctx, verbose):
    board = ctx.obj['board']
    b = Board(board)
    b.build_target('Simulate-test', verbose)


@main.command(help='Build and flash target')
@click.pass_context
def flash(ctx):
    board = ctx.obj['board']
    b = Board(board)
    b.flash()


@main.command(help='Run tests on board')
@click.pass_context
@click.option('-v', '--verbose', is_flag=True)
@click.option('-p', '--port', type=str, help='Port the board is on')
def test(ctx, verbose, port):
    board = ctx.obj['board']
    b = Board(board, port)
    b.test(verbose)


if __name__ == '__main__':
    main(obj={})
