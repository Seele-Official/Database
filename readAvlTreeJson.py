import graphviz
import json



# 创建一个Graphviz的有向图对象


# 递归添加节点和边
def add_nodes_edges(tree, parent=None):
    if tree is None:
        return
    node_label = f"{tree['data']}\nheight={tree['height']}"
    dot.node(node_label)
    if parent:
        dot.edge(parent, node_label)
    if tree['left']:
        add_nodes_edges(tree['left'], node_label)
    if tree['right']:
        add_nodes_edges(tree['right'], node_label)
if __name__ == '__main__':
    while True:
        dot = graphviz.Digraph()
        tree = {}
        with open('tree.json', 'r', encoding='utf-8') as f:
            tree = json.load(f)
        # 从根节点开始添加
        add_nodes_edges(tree)

        # 渲染并展示图像
        dot.render('tree', format='png', view=True)
        if input("Continue? (y/n)") == 'n':
            break
