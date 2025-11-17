import json
import pandas as pd
import matplotlib.pyplot as plt

def draw_table(json_path: str, output_path: str):
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    results = data["results"]

    table = pd.DataFrame([{
        "RCL size": r["rcl_size"],
        "Mean": r["mean"],
        "Best": r["best"],
        "Worst": r["worst"]
    } for r in results])

    fig, ax = plt.subplots(figsize=(6, 0.6 * len(table) + 1))

    ax.axis('off')

    table_plot = ax.table(
        cellText=table.values,
        colLabels=table.columns,
        cellLoc='center',
        loc='center'
    )

    table_plot.auto_set_font_size(False)
    table_plot.set_fontsize(10)
    table_plot.scale(1, 1.5)

    plt.savefig(output_path, dpi=200, bbox_inches='tight')
    print(f"Таблица сохранена в {output_path}")


if __name__ == "__main__":
    draw_table("../data/result.json", "../data/table.png")
