import 'package:flutter/material.dart';
import 'package:shms/http.dart';

class WaterPage extends StatefulWidget {
  final DateTime date;
  WaterPage({Key? key, required this.date}) : super(key: key);

  @override
  State<StatefulWidget> createState() => _WaterPageState();
}

class _WaterPageState extends State<WaterPage> {
  bool isLoading = true;
  bool isError = false;
  late WaterModelWrapper data;
  @override
  void initState() {
    getData();
    super.initState();
  }

  @override
  void didUpdateWidget(covariant WaterPage oldWidget) {
    getData();
    super.didUpdateWidget(oldWidget);
  }

  void getData() async {
    setState(() => isLoading = true);
    try {
      var tempData = await getWaterData(
          widget.date.year.toString(),
          widget.date.month.toString().padLeft(2, "0"),
          widget.date.day.toString().padLeft(2, "0"));
      setState(() {
        data = tempData;
        isError = false;
      });
    } catch (_) {
      setState(() => isError = true);
    }
    setState(() => isLoading = false);
  }

  @override
  Widget build(BuildContext context) {
    return isLoading
        ? const Center(
            child: CircularProgressIndicator(),
          )
        : isError
            ? Center(
                child: Column(
                  children: [
                    const Text("Something Went Wrong"),
                    ElevatedButton(
                        onPressed: getData, child: const Text("Retry"))
                  ],
                ),
              )
            : Padding(
                padding:
                    const EdgeInsets.symmetric(horizontal: 64, vertical: 32),
                child: Flex(
                  direction: Axis.vertical,
                  children: [
                    SizedBox(
                      width: double.infinity,
                      child: Card(
                        child: Container(
                          padding: const EdgeInsets.all(32),
                          child: Column(
                            children: [
                              const Text(
                                "Total Consumtion",
                                style: TextStyle(fontSize: 48),
                              ),
                              const SizedBox(
                                height: 8,
                              ),
                              Text(
                                "${data.total} ml",
                                style: const TextStyle(
                                    fontWeight: FontWeight.w700, fontSize: 32),
                              ),
                              const SizedBox(
                                height: 8,
                              ),
                              Text(
                                  "${widget.date.year}/${widget.date.month.toString().padLeft(2, "0")}/${widget.date.day.toString().padLeft(2, "0")}",
                                  style: const TextStyle(fontSize: 18))
                            ],
                          ),
                        ),
                      ),
                    ),
                    const SizedBox(
                      height: 16,
                    ),
                    Expanded(
                        child: ListView.separated(
                            itemBuilder: (c, i) => ListTile(
                                  title: Text(data.data[i].a > 0
                                      ? "Drink ${data.data[i].a}"
                                      : "Filled ${data.data[i].a.abs()}"),
                                  subtitle: Text(data.data[i].date),
                                  trailing: const Icon(Icons.local_drink),
                                ),
                            separatorBuilder: (c, i) => const Divider(),
                            itemCount: data.data.length))
                  ],
                ),
              );
  }
}
