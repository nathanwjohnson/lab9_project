#!/usr/bin/python3
import argparse
import sys
import os
import subprocess
import re
import serial
import shutil
import pathlib

VITIS_BIN = "/opt/Xilinx/Vitis/2019.2/bin/vitis"
XSCT_BIN_WINDOWS = "C:/Xilinx/Vitis/2020.2/bin/xsct"

LABS_DIR = pathlib.Path(__file__).resolve().parent


class TermColors:
    """Terminal codes for printing in color"""

    PURPLE = "\033[95m"
    BLUE = "\033[94m"
    GREEN = "\033[92m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    END = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def print_color(color, *msg):
    """Print a message in color"""
    print(color + " ".join(str(item) for item in msg), TermColors.END)


def error(*msg, returncode=-1):
    """Print an error message and exit program"""
    print_color(TermColors.RED, "ERROR:", *msg)
    sys.exit(returncode)


def find_serial(interface_id):
    """This function finds device files in /dev that are named ttyUSB* and are using interface 1.
    If 0 or multiple files are found, this function throws an error.
    """
    matches = []
    for dev_path in pathlib.Path("/dev").glob("ttyUSB*"):
        # print(str(dev_path))

        p = subprocess.run(
            ["udevadm", "info", "-q", "path", "-n", dev_path], stdout=subprocess.PIPE
        )

        m = re.search(r"\." + str(interface_id) + r"/ttyUSB", p.stdout.decode())
        if not m:
            continue
        matches.append(dev_path)

    if not matches:
        error(
            "No board could be found.  Make sure the board is plugged in and powered on.  You can also try powering it off and back on."
        )
    if len(matches) > 1:
        error(
            "Multiple boards connected ("
            + ",".join([str(p) for p in matches])
            + "). Make sure only the Zybo board is plugged in."
        )
    return matches[0]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--windows",
        action="store_true",
        help="This flag indicates the Xilinx tools are installed in Windows",
    )
    parser.add_argument(
        "--xilinx_programmer",
        action="store_true",
        help="Use the Xilinx programmer instead of the openocd programmer.",
    )
    parser.add_argument("elf", help="The elf file to run (ex. 'lab1/lab1.elf')")
    args = parser.parse_args()

    elf_path = pathlib.Path(args.elf).absolute()

    if not elf_path.is_file():
        error("File", elf_path, "does not exist")

    # Find the serial device file (Zybo uses interface 1)
    serial_path = None
    if not args.windows:
        serial_path = find_serial(interface_id=1)

    # Open serial device in nonblocking mode
    if serial_path:
        print_color(TermColors.PURPLE, "Capturing output from serial device", serial_path)
        try:
            ser = serial.Serial(str(serial_path), baudrate=115200, timeout=0)
        except serial.serialutil.SerialException as exc:
            print(exc)
            error("Could not open serial port. Power the board off and on and then try again.")

    print_color(TermColors.PURPLE, "\nProgramming elf file:", elf_path)

    # If running tools from Windows, copy necessary files to Windows directory.
    if args.windows:
        win_temp_path = pathlib.Path("/mnt/c/temp/ecen330")
        win_temp_path.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(elf_path, win_temp_path / "program.elf")
        shutil.copyfile(
            LABS_DIR / "platforms/zybo/xil_arm_toolchain/run_elf_windows.tcl",
            win_temp_path / "run_elf_windows.tcl",
        )
        shutil.copyfile(
            LABS_DIR / "platforms/hw/330_hw_system.bit", win_temp_path / "330_hw_system.bit"
        )
        shutil.copyfile(
            LABS_DIR / "platforms/hw/330_hw_system.xsa", win_temp_path / "330_hw_system.xsa"
        )

    # Execute programming and print output

    if args.windows:
        # Use xilinx programmer in Windows
        cmd = [
            "cmd.exe",
            "/C",
            "cd C:/temp/ecen330 && " + XSCT_BIN_WINDOWS + " C:/temp/ecen330/run_elf_windows.tcl",
        ]
        subprocess.run(cmd, cwd="/mnt/c")
    elif args.xilinx_programmer:
        # Use xilinx programmer
        my_env = os.environ.copy()
        my_env["ELF_FILE"] = elf_path
        cmd = [VITIS_BIN, "-batch", str(LABS_DIR / "platforms/zybo/xil_arm_toolchain/run_elf.tcl")]
        subprocess.run(cmd, cwd=LABS_DIR, env=my_env)

    else:

        # Use openocd programmer
        programmer_path = LABS_DIR / "tools" / "fpga-programmer" / "fpga_programmer.py"
        if not programmer_path.is_file():
            error(
                "fpga-programmer not found.  Did you remember to run 'make setup' from the top-level directory?"
            )
        cmd = [
            programmer_path,
            "zybo",
            "--bit",
            LABS_DIR / "platforms" / "hw" / "330_hw_system.bit",
            "--elf",
            str(elf_path),
        ]
        subprocess.run(cmd)

    if serial_path:
        print_color(TermColors.PURPLE + "\nPrinting program output from serial (Ctrl+C to quit)")
        while True:
            try:
                line = ser.readline().decode("ascii", errors="replace")
                if line != "":
                    sys.stdout.write(line)
            except KeyboardInterrupt:
                print("")
                # proc.kill()
                # if args.fileName:
                #     f.close()
                break


if __name__ == "__main__":
    main()
