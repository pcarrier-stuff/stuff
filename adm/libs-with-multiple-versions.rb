#!/usr/bin/env ruby

# Display every library for which different processes use different versions
# Relies extensively on Linux's procfs

# Known issues:
# - openvz makes it utterly useless

require 'pp'

ProcInfo = Struct.new :name, :libs
infos = {}

def libname path
	File.basename(path)[/^\w+(-[a-zA-Z]+)?/]
end

pids = Dir.new("/proc").entries.grep /^[0-9]+$/

pids.each do |p|
	pid = p.to_i
	name = IO::read("/proc/#{pid}/cmdline")[0..-1].gsub /\0/, ' '
	liblines = 
	libs = Hash[
		IO::read("/proc/#{pid}/maps").lines.grep(/\.so/).collect do |line|
			fields = line.split
			devinode = "#{fields[3]} #{fields[4]}"
			path = fields[5..-1].join ' '
			next [devinode, path]
		end
	]
	infos[pid] = ProcInfo.new name, libs
end

libs = {}
infos.each do |pid, info|
	info[:libs].each do |devinode, path|
		name = libname path
		descr = "[#{devinode}]#{path}"
		libs[name] ||= {}
		libs[name][descr] ||= []
		libs[name][descr] << pid
	end
end

puts "Libraries: #{libs.keys.join ','}"

libs.reject { |name, versions| versions.length == 1 }.each do |name, versions|
	$stderr.puts "#{name} has multiple versions:"
	versions.each do |descr, pids|
		$stderr.puts "  #{descr} used by PIDs #{pids.join ','}"
	end
end