import matplotlib.pyplot as plt
import matplotlib.patches as patches

def draw_family_tree():
    fig, ax = plt.subplots(figsize=(10, 8))
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.axis('off')  # 隐藏坐标轴

    # 定义节点坐标 (x, y, 姓名, 边框颜色)
    # 祖父母层 y=8
    nodes = {
        "grandpa": (5, 8, "爷爷\n1930-2010", "#6c8ebf"),
        "grandma": (7, 8, "奶奶\n1932-2018", "#6c8ebf"),
        "dad": (3, 5, "爸爸\n1955", "#82b366"),
        "mom": (5, 5, "妈妈\n1956", "#82b366"),
        "uncle": (7, 5, "叔叔\n1958", "#82b366"),
        "aunt": (9, 5, "婶婶\n1960", "#82b366"),
        "me": (2, 2, "我\n1985", "#9673a6"),
        "sister": (4, 2, "妹妹\n1988", "#9673a6"),
        "cousin": (8, 2, "堂弟\n1990", "#9673a6"),
    }

    # 定义边 (起点, 终点)
    edges = [
        ("grandpa", "grandma"),  # 夫妻连线
        ("dad", "mom"),
        ("uncle", "aunt"),
        ("grandpa", "dad"),
        ("grandpa", "uncle"),
        ("dad", "me"),
        ("dad", "sister"),
        ("uncle", "cousin"),
    ]

    # 绘制连线
    for start, end in edges:
        x1, y1, _, _ = nodes[start]
        x2, y2, _, _ = nodes[end]
        # 夫妻连线用灰色实线，无箭头；亲子连线用黑色实线，箭头可选
        if start in ["grandpa", "dad", "uncle"] and end in ["grandma", "mom", "aunt"]:
            ax.plot([x1, x2], [y1, y2], color='gray', linewidth=2, linestyle='-')
        else:
            ax.annotate("", xy=(x2, y2-0.1), xytext=(x1, y1+0.1),
                        arrowprops=dict(arrowstyle="-|>", color='black', lw=1.5))

    # 绘制节点（矩形框 + 文字）
    for _, (x, y, label, color) in nodes.items():
        # 矩形框
        rect = patches.FancyBboxPatch((x-0.8, y-0.4), 1.6, 0.8,
                                      boxstyle="round,pad=0.1",
                                      facecolor=color, alpha=0.3,
                                      edgecolor=color, linewidth=2)
        ax.add_patch(rect)
        # 文字
        ax.text(x, y, label, ha='center', va='center', fontsize=10, fontweight='bold')

    plt.tight_layout()
    plt.savefig('family_tree.png', dpi=150, bbox_inches='tight')
    plt.show()
    print("家族树已保存为 family_tree.png")

if __name__ == "__main__":
    draw_family_tree()