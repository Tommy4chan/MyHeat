import { Functions } from '@/components/Functions';
import { FunctionSettings } from './components/FunctionSettings';

const FunctionsPage = () => {
  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 items-start gap-6">
      <div className="lg:col-span-2">
        <Functions isTwoCols={true} />
      </div>
      <FunctionSettings />
    </div>
  );
};

export default FunctionsPage;