import graph_cpp

V = 20
beer_prob = 0.2

# build and preprocess graph
G = graph_cpp.graph(V, beer_prob)
G.build_dual(1)
G.build_graph()
G.preprocess()

D = graph_cpp.dag()


########################################################################################
# build vertex elements json
list_vertex_elements = list()
traverse = G.root_edge.twin
for i in range(V):
    map_vertex_element = dict()
    map_vertex_element["data"] = dict()
    map_vertex_element["data"]["id"] = traverse.target.data
    map_vertex_element["data"]["label"] = traverse.target.data
    map_vertex_element["data"]["beer"] = traverse.target.beer
    map_vertex_element["data"]["dist"] = traverse.target.dist
    map_vertex_element["data"]["distB"] = traverse.target.distB
    map_vertex_element["data"]["in_dag"] = False
    map_vertex_element["data"]["in_path"] = False

    map_vertex_element["position"] = dict()
    map_vertex_element["position"]["x"] = traverse.target.coord[0] + V
    map_vertex_element["position"]["y"] = traverse.target.coord[1] + V

    list_vertex_elements.append(map_vertex_element)
    traverse = traverse.next

# build edge element json
list_edge_elements = list()
traverse = G.root_edge.twin
for i in range(V):
	for adj in traverse.target.adjacency:
		map_edge_element = dict()
		map_edge_element["data"] = dict()
		map_edge_element["data"]["source"] = adj.prev.target.data
		map_edge_element["data"]["target"] = adj.target.data
		map_edge_element["data"]["weight"] = adj.weight

		list_edge_elements.append(map_edge_element)

	traverse = traverse.next


# reset
def reset_stylesheet():
    for v in list_vertex_elements:
        v["data"]["in_dag"] = False
        v["data"]["in_path"] = False



#######################################################################################
# import web app visualize library
import dash
import dash_html_components as html
import dash_core_components as dcc
import dash_cytoscape as cyto
from dash.dependencies import Input, Output, State

import plotly.express as px

import networkx as nx


default_elements = list_vertex_elements + list_edge_elements;

default_stylesheet = [
    {
        'selector': 'node',
        'style': {
            'content': 'data(id)',
            'background-color': 'grey'
        }
    }
]


# build app
app = dash.Dash(__name__)

app.layout = html.Div([
	# dropdown menu for graph layout
    dcc.Dropdown(
        id = 'dropdown-update-layout',
        value = 'circle',
        clearable = False,
        options = [
            {'label': name, 'value': name}
            for name in ['grid', 'random', 'circle', 'cose', 'concentric']
        ]
    ),


    # query source and destination vertex
      html.Div([
        html.Div(style={'width': '50%', 'display': 'inline'}, children=[
            'source:',
            dcc.Input(id='input-source', type='number')
        ]),
        html.Div(style={'width': '50%', 'display': 'inline'}, children=[
            'destination:',
            dcc.Input(id='input-destination', type='number')
        ])
    ]),


    # web app main
    cyto.Cytoscape(
        id = 'cytoscape-main',
        layout = {'name': 'circle'},
        style = {
            'position': 'absolute',
            'width': '100%',
            'height': '100%',
        },
        elements = default_elements,
        stylesheet = default_stylesheet
    )

])



# callback for dropdown menu
@app.callback(Output('cytoscape-main', 'layout'),
              Input('dropdown-update-layout', 'value'))
def update_layout(layout):
    return {
        'name': layout,
        'animate': True
    }


# callback for queried vertices
@app.callback(Output('cytoscape-main', 'stylesheet'),
              Input('input-source', 'value'),
              Input('input-destination', 'value'))
def update_dag(input_source, input_destination):
    reset_stylesheet();
    if input_source is None:
        input_source = 4
    if input_destination is None:
        input_destination = 15

    src = G.get_vertex(input_source)
    dest = G.get_vertex(input_destination)
    D.build(G, src, dest)
    D.shortest_path()
    path = D.print_shortest_path(dest)

    for node in list_vertex_elements:
    	for v in D.mDAG:
    		if node["data"]["id"] == v.data:
    			node["data"]["in_dag"] = True
    	for v in path:
    		if node["data"]["id"] == v.data:
    			node["data"]["in_path"] = True

    new_styles = [
        {   
            'selector': '[?beer]',
            'style': {
                'background-color': '#B6977D'   # brown
            }
        },
        {
            'selector': '[?in_dag]',
            'style': {
                'background-color': '#ADA3D1'
            }
        },
        {
            'selector': '[?in_path]',
            'style': {
                'background-color': '#FBB995'
            }
        }
	]

    D.reset()
    return default_stylesheet + new_styles      #



if __name__ == '__main__':
    app.run_server(port = "8080", debug = True, use_reloader=False)




# import dash
# import dash_cytoscape as cyto
# import dash_html_components as html
# import dash_core_components as dcc
# from dash.dependencies import Input, Output

# app = dash.Dash(__name__)


# nodes = [
#     {
#         'data': {'id': short, 'label': label},
#         'position': {'x': 20*lat, 'y': -20*long}
#     }
#     for short, label, long, lat in (
#         ('la', 'Los Angeles', 34.03, -118.25),
#         ('nyc', 'New York', 40.71, -74),
#         ('to', 'Toronto', 43.65, -79.38),
#         ('mtl', 'Montreal', 45.50, -73.57),
#         ('van', 'Vancouver', 49.28, -123.12),
#         ('chi', 'Chicago', 41.88, -87.63),
#         ('bos', 'Boston', 42.36, -71.06),
#         ('hou', 'Houston', 29.76, -95.37)
#     )
# ]

# edges = [
#     {'data': {'source': source, 'target': target}}
#     for source, target in (
#         ('van', 'la'),
#         ('la', 'chi'),
#         ('hou', 'chi'),
#         ('to', 'mtl'),
#         ('mtl', 'bos'),
#         ('nyc', 'bos'),
#         ('to', 'hou'),
#         ('to', 'nyc'),
#         ('la', 'nyc'),
#         ('nyc', 'bos')
#     )
# ]

# elements = nodes + edges


# default_stylesheet = [
#     {
#         'selector': 'node',
#         'style': {
#             'background-color': '#BFD7B5',
#             'label': 'data(label)'
#         }
#     },
#     {
#         'selector': 'edge',
#         'style': {
#             'line-color': '#A3C4BC'
#         }
#     }
# ]


# app.layout = html.Div([
#     html.Div([
#         html.Div(style={'width': '50%', 'display': 'inline'}, children=[
#             'Edge Color:',
#             dcc.Input(id='input-line-color', type='text')
#         ]),
#         html.Div(style={'width': '50%', 'display': 'inline'}, children=[
#             'Node Color:',
#             dcc.Input(id='input-bg-color', type='text')
#         ])
#     ]),

#     cyto.Cytoscape(
#         id='cytoscape-stylesheet-callbacks',
#         layout={'name': 'circle'},
#         stylesheet=default_stylesheet,
#         style={'width': '100%', 'height': '450px'},
#         elements=elements
#     )
# ])


# @app.callback(Output('cytoscape-stylesheet-callbacks', 'stylesheet'),
#               Input('input-line-color', 'value'),
#                Input('input-bg-color', 'value'))
# def update_stylesheet(line_color, bg_color):
#     if line_color is None:
#         line_color = 'transparent'

#     if bg_color is None:
#         bg_color = 'transparent'

#     new_styles = [
#         {
#             'selector': 'node',
#             'style': {
#                 'background-color': bg_color
#             }
#         },
#         {
#             'selector': 'edge',
#             'style': {
#                 'line-color': line_color
#             }
#         }
#     ]

#     return default_stylesheet + new_styles


# if __name__ == '__main__':
#     app.run_server(debug=True)