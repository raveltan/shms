import 'package:flutter/material.dart';
import 'package:shms/http.dart';

class WaterPage extends StatefulWidget {
  final DateTime date;
  const WaterPage({Key? key, required this.date}) : super(key: key);

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
                        shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(20)),
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
                            itemBuilder: (c, i) {
                              var date = DateTime.parse(data.data[i].date);
                              return ListTile(
                                title: Text(
                                  data.data[i].a > 0
                                      ? "Filled ${data.data[i].a}"
                                      : "Drink ${data.data[i].a.abs()}",
                                  style: const TextStyle(fontSize: 18),
                                ),
                                trailing: Text(
                                    "${date.hour.toString().padLeft(2, '0')}:${date.minute.toString().padLeft(2, '0')}"),
                                leading: Icon(data.data[i].a < 0
                                    ? Icons.local_drink
                                    : Icons.no_drinks),
                              );
                            },
                            separatorBuilder: (c, i) => const Divider(),
                            itemCount: data.data.length))
                  ],
                ),
              );
  }
}
