#! /usr/bin/env python3

import sys
import os
import json

def add_compile_arg(args: list[str], arg: str):
	arg = arg.strip()

	if arg and not arg.startswith("//") and not arg.startswith("#"):
		args.append(arg)


def add_compile_command(commands: list[dict], path: str, other_flags: list[str]):
	path = path.strip()

	lang_flags: list[str] = []

	if path.endswith(".c"):
		lang_flags = ["-xc", "--std=c99"]
	elif path.endswith(".h"):
		lang_flags = ["-xc-header", "--std=c99"]
	else:
		return # not code

	command: dict = {}
	command["directory"] = os.getcwd()
	command["file"] = path
	command["arguments"] = ["clangd"] + lang_flags + other_flags
	commands.append(command)


def main():
	if len(sys.argv) == 1:
		print("Usage: " + sys.argv[0] + " compile_flags <compile_commands>")
		return

	compile_flags: list[str] = []
	compile_commands: list[dict] = []

	with open(sys.argv[1], "r") as file:
		for line in file:
			add_compile_arg(compile_flags, line)

	for path, dirs, files in os.walk("source"):
		for file in files:
			add_compile_command(compile_commands, os.path.join(path, file), compile_flags)

	for path, dirs, files in os.walk("include"):
		for file in files:
			add_compile_command(compile_commands, os.path.join(path, file), compile_flags)


	outfile: str = "compile_commands.json"
	if len(sys.argv) - 1 >= 2 and sys.argv[2]:
		outfile = sys.argv[2]

	with open(outfile, "w") as file:
		file.write(json.dumps(compile_commands, indent = 2))


if __name__ == "__main__":
	main()
