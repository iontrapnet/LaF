var edge = require('edge');

var PlotSurface2D = edge.func({source : function() {/*
        #r "NPlot.dll"
        using System.Threading.Tasks;
        using System.Windows.Forms;
        using NPlot.Windows;

        public class Startup
        {
            public async Task<object> Invoke(object input)
            {   
                var surf = new PlotSurface2D();
                var frm = new Form();
                frm.Dock = DockStyle.Fill;
                frm.Controls.Add(surf);
                var line = new NPlot.LinePlot();
                line.AbscissaData = new double[]{1,2,3,4};
                line.OrdinateData = input;
                surf.Add(line);
                frm.ShowDialog();
                return 3;
            }
        }
*/},
references : [ 'System.Windows.Forms.dll' ]});
PlotSurface2D([10,25,30,100], function (err, res) {
    if (err) throw err;
    console.log(res);
});


