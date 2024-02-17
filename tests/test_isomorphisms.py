from hypothesis import given, strategies as st
import pytest

import bliss_bind as bb


@pytest.fixture
def g1():
    g = bb.Graph()
    g.add_vertex(color=0)  # 0
    g.add_vertex(color=0)  # 1
    g.add_vertex(color=0)  # 2
    g.add_edge(0, 1)
    g.add_edge(0, 2)
    g.add_edge(1, 2)
    return g


@pytest.fixture
def g2():
    g = bb.Graph()
    g.add_vertex(color=1)
    g.add_vertex(color=0)
    g.add_vertex(color=0)
    g.add_edge(0, 1)
    g.add_edge(0, 2)
    g.add_edge(1, 2)
    return g


@pytest.fixture
def g3():
    g = bb.Graph()
    g.add_vertex(color=1)
    g.add_vertex(color=1)
    g.add_vertex(color=0)
    g.add_edge(0, 1)
    g.add_edge(0, 2)
    g.add_edge(1, 2)
    return g


def test_find_automorphisms(g1, g2, g3):
    perms = []
    s = g1.find_automorphisms(callback=lambda x: perms.append(tuple(x)))
    assert isinstance(s, bb.Stats)
    assert len(perms) == 2
    assert set(perms) == {(0, 2, 1), (1, 0, 2)}

    perms = []
    s = g2.find_automorphisms(callback=lambda x: perms.append(tuple(x)))
    assert isinstance(s, bb.Stats)
    assert len(perms) == 1
    assert set(perms) == {(0, 2, 1)}

    perms = []
    s = g3.find_automorphisms(callback=lambda x: perms.append(tuple(x)))
    assert isinstance(s, bb.Stats)
    assert len(perms) == 1
    assert set(perms) == {(1, 0, 2)}


# A strategy for generating a list of edges that form a valid graph
def graph_edges(node_count):
    return st.lists(
            st.lists(st.integers(min_value=0, max_value=node_count-1),
                     min_size=2, max_size=2, unique=True),
            min_size=0, max_size=node_count*(node_count-1)//2,
            unique_by=lambda x: (min(x), max(x)))


def vertex_colors(node_count, color_range=(1, 3)):
    min_color, max_color = color_range
    return st.lists(st.integers(min_value=min_color, max_value=max_color),
                    min_size=node_count, max_size=node_count)


@given(node_count=st.integers(min_value=2, max_value=20),
       edge_strategy=st.data(), color_strategy=st.data())
def test_arbitrary_graph_automorphisms(
        node_count, edge_strategy, color_strategy):
    edges = edge_strategy.draw(graph_edges(node_count))
    colors = color_strategy.draw(vertex_colors(node_count))
    g = bb.Graph()
    for i in colors:
        g.add_vertex(color=i)
    for e in edges:
        g.add_edge(*e)

    perms = []
    g.find_automorphisms(callback=lambda x: perms.append(x))
    for p in perms:
        assert g.is_automorphism(p)

    perms = []
    g.canonical_form(callback=lambda x: perms.append(x))
    for p in perms:
        assert g.is_automorphism(p)


@given(node_count=st.integers(min_value=2, max_value=20),
       edge_strategy=st.data(), color_strategy=st.data())
def test_arbitrary_graph_canonical_form(
        node_count, edge_strategy, color_strategy):
    edges = edge_strategy.draw(graph_edges(node_count))
    colors = color_strategy.draw(vertex_colors(node_count))
    g = bb.Graph()
    for i in colors:
        g.add_vertex(color=i)
    for e in edges:
        g.add_edge(*e)

    canon, _ = g.canonical_form()
    perms = []
    g.find_automorphisms(callback=lambda x: perms.append(x))
    for p in perms:
        assert g.permute(p).canonical_form()[0] == canon
