## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('geoflow', ['internet', 'wifi', 'applications'])
    module.includes = '.'
    module.source = [
    	'model/geoflow-packetentry.cc',
    	'model/geoflow-packetqueue.cc',
    	'model/geoflow-routing.cc',
        'model/geoflow-controller.cc',
        'model/geoflow-protocol.cc',
        'model/geoflow-message.cc',
        'model/geoflow-copy.cc',
        'model/geoflow-agent.cc',
        'model/geoflow-header.cc',
        'model/geoflow-switch.cc',
        'model/geoflow-topology.cc',
        'model/geoflow-multicopy-routing.cc',
        'model/geoflow-forward.cc',
        'model/geoflow-neighbors.cc',
        'model/geoflow-flowtable.cc',
        'model/geoflow-flowentry.cc',
		'helper/geoflow-helper.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'geoflow'
    headers.source = [
    	'model/geoflow-packetentry.h',
    	'model/geoflow-packetqueue.h',
    	'model/geoflow-routing.h',
        'model/geoflow-controller.h',
        'model/geoflow-protocol.h',
        'model/geoflow-message.h',
        'model/geoflow-copy.h',
        'model/geoflow-agent.h',
        'model/geoflow-header.h',
        'model/geoflow-switch.h',
        'model/geoflow-topology.h',
        'model/geoflow-multicopy-routing.h',
        'model/geoflow-forward.h',
        'model/geoflow-neighbors.h',
        'model/geoflow-flowtable.h',
        'model/geoflow-flowentry.h',
        'helper/geoflow-helper.h',
        ]
